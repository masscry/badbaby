#include <cassert>

#include <atomic>

#include <worker.hpp>
#include <common.hpp>
#include <config.hpp>

namespace
{
  void SetThisThreadName(const std::string& name)
  {
    pthread_setname_np(pthread_self(), name.c_str());
  }

  int MakeNewActorID(uint16_t actorIndex)
  {
    //
    // System requires some way to separate deleted actors from active,
    // I decided to use unique IDs.
    //
    // 0xUUUUIIII
    //
    // I - 16-bit index in actors array
    // U - 16-bit always increasing counter
    //
    static std::atomic_uint16_t uid(0);
    return (((uid++) & 0x7FFF) << 16) | actorIndex;
  }

  uint16_t GetActorIndex(int actorID)
  {
    return (actorID & 0xFFFF);
  }

}

namespace bb
{

  static const uint32_t maxActorsInWorkerPool = 0x10000;

  void workerPool_t::PrepareInfo(workerID_t id)
  {
    auto& info = this->infos[id];
    std::unique_lock<std::mutex> lock(info.guard);
    info.stop = false;
    SetThisThreadName(std::string("worker") + std::to_string(id));

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
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
      {
        auto readLock = this->actorsGuard.GetReadLock();
        assert(this->actors.size() <= maxActorsInWorkerPool);

        size_t totalKnownActors = this->actors.size();
        for (size_t curActorIndex = 0; curActorIndex < totalKnownActors; ++curActorIndex)
        {
          auto actorIt = this->actors.begin() + curActorIndex;
          auto actorProcessResult = msgResult_t::skipped;
          auto& actor = *actorIt;
          if (actor)
          {
            actorProcessResult = actor->ProcessMessages();
          }
          switch (actorProcessResult)
          {
          case msgResult_t::skipped:
          case msgResult_t::complete:
            break;
          case msgResult_t::poisoned:
            {
              readLock.reset(); // must get stronger lock
              auto wlock = this->actorsGuard.GetWriteLock();
              this->deletedActors.push_back(curActorIndex);
              actorIt->reset();

              wlock.reset(); // return to normal read lock
              readLock = this->actorsGuard.GetReadLock();
            }
            break;
          case msgResult_t::error:
            bb::Error("Actor \"%s\" (%d) works with errors", actor->Name().c_str(), actor->ID());
            break;
          default:
            assert(0);
          }
        }
      }
      info.notify.wait(lock, [&info, this](){ return (info.stop) || this->HasActorsInQueue(); });
    }
    Info("%s", "Worker Stopped");
  }

  workerPool_t::workerPool_t()
  {
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

    this->infos = std::move(vectorOfInfo_t(totalWorkers));
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
  }

  workerPool_t& workerPool_t::Instance()
  {
    static workerPool_t self;
    return self;
  }

  int workerPool_t::Register(std::unique_ptr<role_t>&& role)
  {
    assert(role);

    auto lock = this->actorsGuard.GetWriteLock();
    if (this->deletedActors.empty() && (this->actors.size() >= maxActorsInWorkerPool))
    {
      bb::Error("%s %u", "Too many actors! Must not be greater than ", maxActorsInWorkerPool);
      return -1;
    }

    std::unique_ptr<actor_t> newActor(new actor_t(std::move(role)));

    int resultActorID = -1;
    if (this->deletedActors.empty())
    {
      uint16_t actorIndex = (this->actors.size() & 0xFFFF);

      resultActorID = MakeNewActorID(actorIndex);
      newActor->PostMessage(IssueSetID(resultActorID));
      newActor->ProcessMessages();
      this->actors.emplace_back(std::move(newActor));
    }
    else
    {
      uint16_t actorIndex = this->deletedActors.front();
      this->deletedActors.pop_front();
      
      resultActorID = MakeNewActorID(actorIndex);
      newActor->PostMessage(IssueSetID(resultActorID));
      newActor->ProcessMessages();
      this->actors[actorIndex].swap(newActor);
    }

    bb::Info("Actor (%08x) registered", resultActorID);
    return resultActorID;
  }

  int workerPool_t::FindFirstByName(const std::string& name)
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

  int workerPool_t::PostMessage(int actorID, msg_t message)
  {
    uint16_t actorIndex = GetActorIndex(actorID);
    assert(actorIndex < this->actors.size());

    {
      auto lock = this->actorsGuard.GetReadLock();

      auto actIt = this->actors.begin() + actorIndex;
      if ((*actIt) && ((*actIt)->ID() == actorID))
      {
        (*actIt)->PostMessage(message);
      }
      else
      {
        bb::Error("Actor (%08x) is no longer exists!", actorID);
        return -1;
      }
    }
    for (auto& info: this->infos)
    {
      info.notify.notify_one();
    }
    return 0;
  }

  int workerPool_t::Unregister(int actorID)
  {
    return this->PostMessage(actorID, bb::MakeMsg(-1, POISON, 0));
  }

}