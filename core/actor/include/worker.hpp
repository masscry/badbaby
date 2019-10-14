/**
 * @file worker.hpp
 * 
 * Class implements thread which processes actors actions.
 * 
 */

#pragma once
#ifndef __BB_CORE_ACTOR_WORKER_HEADER__
#define __BB_CORE_ACTOR_WORKER_HEADER__

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <list>
#include <atomic>
#include <unordered_map>

#include <common.hpp>
#include <actor.hpp>
#include <mailbox.hpp>

namespace bb
{

  class workerPool_t final
  {
    struct workerInfo_t final
    {
      std::mutex              guard;
      std::condition_variable notify;
      bool                    stop;
    };

    struct actorInfo_t final
    {
      std::unique_ptr<mailbox_t> mailbox;
      std::unique_ptr<actor_t>   actor;
      std::atomic_int            ownerID;
    };

    using workerID_t = unsigned int;
    using vectorOfWorkers_t = std::vector<std::thread>;
    using vectorOfInfo_t = std::vector<workerInfo_t>;
    using actorStorage_t = std::unordered_map<std::string, std::unique_ptr<actorInfo_t>>;

    void PrepareInfo(workerID_t id);
    void WorkerThread(workerID_t id);

    vectorOfWorkers_t workers;
    vectorOfInfo_t    infos;

    rwMutex_t         actorsGuard;
    actorStorage_t    actors;

    std::atomic_int   totalMessages;

    workerPool_t();
    ~workerPool_t();

    workerPool_t(const workerPool_t&) = delete;
    workerPool_t(workerPool_t&&) = delete;

    workerPool_t& operator = (const workerPool_t&) = delete;
    workerPool_t& operator = (workerPool_t&&) = delete;

  public:

    static workerPool_t& Instance();

    void Register(const std::string& name, std::unique_ptr<actor_t> actor);
    void Unregister(const std::string& name);

    void PostMessage(const std::string& name, msg_t message);

  };

} // namespace bb

#endif /* __BB_CORE_ACTOR_WORKER_HEADER__ */