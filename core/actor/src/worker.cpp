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
        if (this->globalTotalMessages != 0)
        {
          Info("Wait for %d messages to finish", static_cast<int>(this->globalTotalMessages));
        }
        else
        {
          break;
        }
      }
      {
        auto lock = this->actorsGuard.GetReadLock();
        size_t totalKnownActors = this->actors.size();
        for (size_t curActorID = 0; curActorID < totalKnownActors; ++curActorID)
        {
          auto actorIt = this->actors.begin() + curActorID;

          if (*actorIt)
          {
            auto& curActor = *actorIt;

            uint16_t expected = SOI_FREE;
            if (std::atomic_compare_exchange_strong(&curActor->ownerID, &expected, id))
            { // actor captured
              lock.reset(); // release lock to give others to do changes in other places

              size_t actorTotalMessages = curActor->mailbox->Has();
              this->globalTotalMessages -= actorTotalMessages;
              bb::Info("Process %lu messages for %s", actorTotalMessages, curActor->actor->Name().c_str());
              while (actorTotalMessages-->0)
              {
                auto msg = curActor->mailbox->Wait();
                switch (msg.type)
                {
                  case SET_ID:
                    curActor->actor->SetPoolID(this, bb::GetMsgData<int>(msg));
                    break;
                  case POISON:
                    {
                      auto writeLock = this->actorsGuard.GetWriteLock();
                      auto& context = bb::context_t::Instance();

                      context.UnregisterActorCallbacks(curActor->actor->ID());
                      this->deletedActors.emplace_back(actorIt);
                      curActor->actor->SetPoolID(nullptr, -1);
                      curActor.reset();
                      writeLock.reset();
                      actorTotalMessages = 0; // this stop loop
                      continue;     // this goes to loop condition check
                    }
                  default:
                    assert(curActor->actor->ID() >= 0);
                    curActor->actor->ProcessMessage(msg);
                }
              }

              if (curActor)
              { // When actor poisoned, no owner to remove
                curActor->ownerID = -1; // actor released
              }
              lock = this->actorsGuard.GetReadLock();
            }
          }
        }
      }
      info.notify.wait(lock, [&info, this](){ return (info.stop) || (this->globalTotalMessages > 0); });
    }
    Info("%s", "Worker Stopped");
  }

  workerPool_t::workerPool_t()
  :globalTotalMessages(0)
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
    actInfo->ownerID.store(SOI_FREE);

    if (this->deletedActors.empty())
    {
      this->actors.emplace_back(std::move(actInfo));
      int result = this->actors.size() - 1;
      this->actors.back()->mailbox->Put(bb::MakeMsg(-1, SET_ID, result));
      return result;
    }
    else
    {
      actorStorage_t::iterator first = this->deletedActors.front();
      this->deletedActors.pop_front();
      (*first).swap(actInfo);
      int result = first - this->actors.begin();
      (*first)->mailbox->Put(bb::MakeMsg(-1, SET_ID, result));
      return result;
    }
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
        auto& actor = *actIt;
        actor->mailbox->Put(std::move(message));
        this->globalTotalMessages++;
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