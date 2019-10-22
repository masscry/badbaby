#include <atomic>

#include <worker.hpp>
#include <common.hpp>

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
          if (std::atomic_compare_exchange_strong(&actor.second->ownerID, &expected, static_cast<int>(id)))
          { // actor captured
            size_t totalMsg = actor.second->mailbox->Has();
            while (totalMsg-->0)
            {
              auto msg = actor.second->mailbox->Wait();
              actor.second->actor->ProcessMessage(msg);
              this->totalMessages--;
            }
            actor.second->ownerID = -1; // actor released
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
    auto totalWorkers = std::thread::hardware_concurrency() - 1;

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

  void workerPool_t::Unregister(const std::string& name)
  {
    auto lock = this->actorsGuard.GetWriteLock();
    auto act = this->actors.find(name);
    if (act != this->actors.end())
    {
      this->actors.erase(act);
    }
  }

  void workerPool_t::Register(const std::string& name, std::unique_ptr<actor_t> actor)
  {
    auto lock = this->actorsGuard.GetWriteLock();
    if (this->actors.find(name) != this->actors.end())
    {
      throw std::runtime_error(std::string("Actor '") + name + "' already exists!");
    }

    std::unique_ptr<actorInfo_t> actInfo(new actorInfo_t);

    actInfo->actor = std::move(actor);
    actInfo->mailbox.reset(new mailbox_t);
    actInfo->ownerID.store(-1);

    this->actors.emplace(name, std::move(actInfo));
  }

  void workerPool_t::PostMessage(const std::string& name, msg_t message)
  {
    auto lock = this->actorsGuard.GetReadLock();
    auto act = this->actors.find(name);
    if (act == this->actors.end())
    {
      throw std::runtime_error(std::string("Actor '") + name + "' not found!");
    }
    act->second->mailbox->Put(std::move(message));
    this->totalMessages++;

    for (auto& info: this->infos)
    {
      info.notify.notify_one();
    }
  }


}