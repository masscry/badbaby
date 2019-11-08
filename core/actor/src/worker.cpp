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

  void workerPool_t::WorkerThread(workerID_t id)
  {
    auto& info = this->infos[id];
    this->PrepareInfo(id);

    Info("%s", "Worker Started");
    while(true)
    {
      std::unique_lock<std::mutex> lock(info.guard);
      if (info.stop)
      {
        Info("%s", "Stop Requested");
        break;
      }
      {
        auto lock = this->actorsGuard.GetReadLock();
        for(auto& actor: this->actors)
        {
          int expected = -1;
          if (std::atomic_compare_exchange_strong(&actor->ownerID, &expected, static_cast<int>(id)))
          { // actor captured
            size_t totalMsg = actor->mailbox->Has();
            while (totalMsg-->0)
            {
              auto msg = actor->mailbox->Wait();
              actor->actor->ProcessMessage(msg);
              this->totalMessages--;
            }
            actor->ownerID = -1; // actor released
          }
        }
      }
      info.notify.wait(lock, [&info, this](){ return (info.stop) || (this->totalMessages != 0); });
    }
    Info("%s", "Worker Stopped");
  }

  workerPool_t::workerPool_t()
  :totalMessages(0)
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

  int workerPool_t::Register(std::unique_ptr<actor_t> actor)
  {
    assert(actor);

    auto lock = this->actorsGuard.GetWriteLock();
    std::unique_ptr<actorInfo_t> actInfo(new actorInfo_t);

    actInfo->actor = std::move(actor);
    actInfo->mailbox.reset(new mailbox_t);
    actInfo->ownerID.store(-1);

    if (this->deletedActors.empty())
    {
      this->actors.emplace_back(std::move(actInfo));
      return this->actors.size() - 1;
    }
    else
    {
      actorStorage_t::iterator first = this->deletedActors.front();
      this->deletedActors.pop_front();
      (*first) = std::move(actInfo);
      return first - this->actors.begin();
    }
  }

  void workerPool_t::Unregister(int id)
  {
    size_t uid = static_cast<size_t>(id);

    auto lock = this->actorsGuard.GetWriteLock();
    if (uid > this->actors.size())
    {
      throw std::runtime_error("Unregister: Invalid actor ID");
    }

    this->deletedActors.emplace_back(this->actors.begin() + uid);
    (this->actors.begin() + uid)->reset();
  }

  int workerPool_t::FindFirstByName(const std::string name)
  {
    auto lock = this->actorsGuard.GetReadLock();
    int index = 0;
    for(auto& actIt: this->actors)
    {
      if ((actIt) && (actIt->actor->Name() == name))
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
        (*actIt)->mailbox->Put(std::move(message));
        this->totalMessages++;
      }
    }
    for (auto& info: this->infos)
    {
      info.notify.notify_one();
    }
  }


}