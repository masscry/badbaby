#include <cassert>

#include <atomic>

#include <mailbox.hpp>
#include <worker.hpp>
#include <common.hpp>
#include <config.hpp>
#include <role.hpp>

namespace
{
#ifdef __APPLE__

  void SetThisThreadName(const std::string& name)
  {
    // macOS threads can set names only to themselves
    pthread_setname_np(name.c_str());
  }
#define HAS_SetThisThreadName
#endif

#ifdef __linux__
  void SetThisThreadName(const std::string& name)
  {
    pthread_setname_np(pthread_self(), name.c_str());
  }
#define HAS_SetThisThreadName
#endif

#ifdef _WIN32
  /** @see https://docs.microsoft.com/ru-ru/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2019 */
  /* Threre are two ways to implement this functionality:
   *  1) SetThreadDescription available from Windows10
   *  2) Throw special exception (available only in debugger)
   *
   * I copied (2), because it is still better, than have no thread name at all
   * and there is no way I can be sure, that engine won't be used in older Windows.
   *
   * Log names depends on thread names, but actually - it is all just for debugging
   *
   */

  static const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
  } THREADNAME_INFO;
#pragma pack(pop)

  void SetThisThreadName(const std::string& name) 
  {
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name.c_str();
    info.dwThreadID = -1;
    info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
    __try 
    {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)& info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) 
    {
      ;
    }
#pragma warning(pop)
  }
#define HAS_SetThisThreadName
#endif

#ifndef HAS_SetThisThreadName
#error "SetThisThreadName undefined for given platform!"
#endif /* HAS_SetThisThreadName */
}

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

        for (auto actorIt = this->actors.begin(), actorEnd = this->actors.end(); actorIt != actorEnd; ++actorIt)
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
          case msg::result_t::skipped:
          case msg::result_t::complete:
            break;
          case msg::result_t::poisoned:
            {
              // this is only way to delete actor, code works in a way that
              // only actor itself says when it can be killed.
              this->actorsGuard.UnlockRead(); // must get stronger lock temporaly

              // read lock will be aquired, when wlock is dies
              BB_DEFER(this->actorsGuard.LockRead());

              // wlock dies before BB_DEFER executes
              auto wlock = this->actorsGuard.GetWriteLock();
              actorIt = this->actors.erase(actorIt);
            }
            break;
          case msg::result_t::error:
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
  }

  workerPool_t& workerPool_t::Instance()
  {
    static workerPool_t self;
    return self;
  }

  int workerPool_t::Register(std::unique_ptr<role_t>&& role)
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
    int resultActorID = newActor->ID();
    this->actors.emplace_back(std::move(newActor));
    
    bb::Info("Actor \"%s\" (%x) registered", roleName.c_str(), resultActorID);
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

  int workerPool_t::PostMessage(int actorID, msg_t&& message)
  {
    if (bb::postOffice_t::Instance().Post(actorID, std::move(message)) != 0)
    {
      bb::Error("Actor (%08x) is no longer exists!", actorID);
      assert(0);
      return -1;
    }
    for (auto& info: this->infos)
    {
      info.notify.notify_one();
    }
    return 0;
  }

  int workerPool_t::Unregister(int actorID)
  {
    return this->PostMessage(actorID,
      bb::IssuePoison()
    );
  }

}
