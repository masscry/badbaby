#include <cassert>

#include <atomic>

#include <worker.hpp>
#include <common.hpp>
#include <config.hpp>
#include <context.hpp>

namespace
{
  void SetThisThreadName(const std::string& name)
  {
    pthread_setname_np(pthread_self(), name.c_str());
  }
}

namespace bb
{

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
        size_t totalKnownActors = this->actors.size();
        for (size_t curActorID = 0; curActorID < totalKnownActors; ++curActorID)
        {
          auto actorIt = this->actors.begin() + curActorID;
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

              context_t::Instance().UnregisterActorCallbacks(curActorID);
              this->deletedActors.push_back(curActorID);
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
    std::unique_ptr<actor_t> newActor(new actor_t(std::move(role)));

    if (this->deletedActors.empty())
    {
      int result = this->actors.size();
      newActor->PostMessage(IssueSetID(result));
      this->actors.emplace_back(std::move(newActor));
      return result;
    }
    else
    {
      int result = this->deletedActors.front();
      this->deletedActors.pop_front();
      newActor->PostMessage(IssueSetID(result));
      this->actors[result].swap(newActor);
      return result;
    }
  }

  int workerPool_t::FindFirstByName(const std::string& name)
  {
    auto lock = this->actorsGuard.GetReadLock();
    int index = 0;
    for(auto& actIt: this->actors)
    {
      if ((actIt) && (actIt->Name() == name))
      {
        return index;
      }
      ++index;
    }
    return -1;
  }

  void workerPool_t::PostMessage(int actorID, msg_t message)
  {
    size_t uid = static_cast<size_t>(actorID);
    {
      auto lock = this->actorsGuard.GetReadLock();
      if (uid > this->actors.size())
      {
        throw std::runtime_error("PostMessage: Invalid actor ID");
      }

      auto actIt = this->actors.begin() + uid;
      if (*actIt)
      {
        auto& actor = *actIt;
        actor->PostMessage(message);
      }
    }
    for (auto& info: this->infos)
    {
      info.notify.notify_one();
    }
  }

  void workerPool_t::Unregister(int actorID)
  {
    this->PostMessage(actorID, bb::MakeMsg(-1, POISON, 0));
  }


}