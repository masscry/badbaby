#include <cassert>

#include <atomic>

#include <mailbox.hpp>
#include <worker.hpp>
#include <common.hpp>
#include <config.hpp>
#include <role.hpp>

namespace bb
{

  static const uint32_t maxActorsInWorkerPool = 0x10000;

  /**
   * @todo add affinity settings for macOSX
   */
  void workerPool_t::PrepareInfo(workerID_t id)
  {
    auto& info = this->infos[id];
    std::unique_lock<std::mutex> lock(info.guard);
    info.stop = false;
    SetThisThreadName(std::string("worker") + std::to_string(id));

#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
#endif /* __linux__ */


  }

  bool workerPool_t::HasActorsInQueue()
  {
    auto lock = this->actorsGuard.GetReadLock();
    for (auto& curActor: this->actors)
    {
      if (curActor)
      {
        if (curActor->NeedProcessing())
        {
          return true;
        }
      }
    }
    return false;
  }

  void workerPool_t::DoProcessActors()
  {
    auto readLock = this->actorsGuard.GetReadLock();
    assert(this->actors.size() <= maxActorsInWorkerPool);
  
    for (auto actorIt = this->actors.begin(), actorEnd = this->actors.end(); actorIt != actorEnd;)
    {
      auto actorProcessResult = msg::result_t::skipped;
      auto& actor = *actorIt;
      if (actor)
      {
        //
        // Actor can use PostMessage from inside, so we need somehow
        // release actorsGuard#readLock, but forbid others to mess with
        // this actor while it processes data.
        //
        // We do this inside ProcessMessagesReadReleaseAquire, after
        // actor's own lock is captured, actorsGuard#readLock can be
        // temporaly released, until actor processing completes
        //
        actorProcessResult = actor->ProcessMessagesReadReleaseAquire(this->actorsGuard);
      }
      switch (actorProcessResult)
      {
      default:
        /* programmer's mistake */
        assert(0);
      case msg::result_t::skipped:
      case msg::result_t::complete:
        ++actorIt; // just incrementing
        break;
      case msg::result_t::poisoned:
      {
        // this is only way to delete actor, code works in a way that
        // only actor itself says when it can be killed.
        this->actorsGuard.UnlockRead(); // must get stronger lock temporaly
  
        BB_DEFER(this->actorsGuard.LockRead()); // read lock will be aquired, when wlock is dies
  
        auto wlock = this->actorsGuard.GetWriteLock(); // wlock dies before BB_DEFER executes
        actorIt = this->actors.erase(actorIt); // incrementing by deleting current, and taking next
      }
      break;
      case msg::result_t::error:
        bb::Error("Actor \"%s\" (%ld) works with errors", actor->Name().c_str(), actor->ID());
        ++actorIt; // just incrementing
        break;
      }
    }
  }

  void workerPool_t::WorkerThread(workerID_t id)
  {
    auto& info = this->infos[id];
    this->PrepareInfo(id);

    Info("%s", "Worker Started");
    while(true)
    {
      std::unique_lock<std::mutex> lock(info.guard);
      if (info.stop)
      { // when stop requested, and all messages processed
        Info("%s", "Stop Requested");
        break;
      }
      this->DoProcessActors();
      info.notify.wait(lock, [&info, this](){ return (info.stop) || this->HasActorsInQueue(); });
    }
    Info("%s", "Worker Stopped");
  }

  workerPool_t::workerPool_t(postOffice_t& postOffice)
  : postOffice(postOffice)
  {
    bb::Debug("%s", "Worker Pool Created");
    config_t config;
    try
    {
      config.Load("default.config");
    }
    catch(const std::runtime_error&)
    {
      // ignore file not found error
      config["actor.workers"]  = ref_t::Number(std::thread::hardware_concurrency() - 1);
      config.Save("default.config");
    }

    auto totalWorkers = static_cast<unsigned int>(config.Value(
      "actor.workers",
      std::thread::hardware_concurrency() - 1
    ));

    Info("Total Worker Count: %u", totalWorkers);

    this->infos = vectorOfInfo_t(totalWorkers);
    for (decltype(totalWorkers) i = 0; i < totalWorkers; ++i)
    {
      this->workers.emplace_back(
        std::thread(&workerPool_t::WorkerThread, this, i)
      );
    }
  }

  workerPool_t::~workerPool_t()
  {
    for (auto& info: this->infos)
    {
      {
        std::lock_guard<std::mutex> lock(info.guard);
        info.stop = true;
      }
      info.notify.notify_one();
    }

    for (auto& worker: this->workers)
    {
      worker.join();
    }
    bb::Debug("%s", "Worker Pool Died");
  }

  workerPool_t& workerPool_t::Instance()
  {
    //
    // When workerPool dies, it must cleanup it's actor data
    // each actor delete itself from postOffice, which may already die
    // so here we pre-creating postOffice, so it always be deleted after workerPool_t.
    //
    static workerPool_t self(postOffice_t::Instance());
    return self;
  }

  actorPID_t workerPool_t::Register(std::unique_ptr<role_t>&& role)
  {
    assert(role);
    std::string roleName = role->DefaultName();

    auto lock = this->actorsGuard.GetWriteLock();
    if (this->actors.size() >= maxActorsInWorkerPool)
    {
      bb::Error("%s %u", "Too many actors! Must not be greater than ", maxActorsInWorkerPool);
      return -1;
    }

    std::unique_ptr<actor_t> newActor(new actor_t(std::move(role)));
    auto resultActorID = newActor->ID();
    this->actors.emplace_back(std::move(newActor));
    
    bb::Info("Actor \"%s\" (%lx) registered", roleName.c_str(), resultActorID);
    return resultActorID;
  }

  actorPID_t  workerPool_t::FindFirstByName(const std::string& name)
  {
    auto lock = this->actorsGuard.GetReadLock();
    for(auto& actIt: this->actors)
    {
      if ((actIt) && (actIt->Name() == name))
      {
        return actIt->ID();
      }
    }
    return -1;
  }

  static inline postAddress_t ActorIDToPostbox(actorPID_t pid)
  { // postAddress_t - lower part of actorID
    // when actorID == -1 - this is an programmer's mistake
    assert(pid != INVALID_ACTOR);
    return static_cast<postAddress_t>(pid);
  }

  int workerPool_t::PostMessage(actorPID_t actorID, msg_t&& message)
  {
    if (actorID == INVALID_ACTOR)
    {
      bb::Error("%s", "Can't post message to INVALID_ACTOR");
      assert(0);
      return -1;
    }

    if (this->postOffice.Post(ActorIDToPostbox(actorID), std::move(message)) != 0)
    {
      bb::Error("Actor (%08lx) is no longer exists!", actorID);
      assert(0);
      return -1;
    }
    for (auto& info: this->infos)
    {
      info.notify.notify_one();
    }
    return 0;
  }

  int workerPool_t::Unregister(actorPID_t actorID)
  {
    return this->PostMessage(actorID,
      bb::IssuePoison()
    );
  }

}
