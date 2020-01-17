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
#include <deque>

#include <common.hpp>
#include <actor.hpp>
#include <mailbox.hpp>

namespace bb
{

  class workerPool_t final
  {

    enum specialOwnerID_t: uint16_t
    {
      SOI_DELETED = 0xFFFE,
      SOI_FREE    = 0xFFFF
    };

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
      std::atomic_uint16_t       ownerID;
    };

    using workerID_t = uint16_t;
    using vectorOfWorkers_t = std::vector<std::thread>;
    using vectorOfInfo_t = std::vector<workerInfo_t>;

    using actorStorage_t = std::vector<std::unique_ptr<actorInfo_t>>;
    using deletedActorList_t = std::list<actorStorage_t::iterator>; 

    void PrepareInfo(workerID_t id);
    void WorkerThread(workerID_t id);

    vectorOfWorkers_t workers;
    vectorOfInfo_t    infos;

    rwMutex_t          actorsGuard;
    actorStorage_t     actors;
    deletedActorList_t deletedActors;

    std::atomic_int   globalTotalMessages;

    workerPool_t();
    ~workerPool_t();

    workerPool_t(const workerPool_t&) = delete;
    workerPool_t(workerPool_t&&) = delete;

    workerPool_t& operator = (const workerPool_t&) = delete;
    workerPool_t& operator = (workerPool_t&&) = delete;

  public:

    static workerPool_t& Instance();

    int Register(std::unique_ptr<actor_t> actor);
    int FindFirstByName(const std::string name);

    void PostMessage(int actorID, msg_t message);
    void Unregister(int actorID);

  };

} // namespace bb

#endif /* __BB_CORE_ACTOR_WORKER_HEADER__ */