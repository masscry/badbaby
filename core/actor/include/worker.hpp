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
#include <list>

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

    using workerID_t = uint16_t;
    using vectorOfWorkers_t = std::vector<std::thread>;
    using vectorOfInfo_t = std::vector<workerInfo_t>;

    using actorStorage_t = std::list<std::unique_ptr<actor_t>>;
    using deletedActorList_t = std::deque<uint16_t>; 

    void PrepareInfo(workerID_t id);
    void DoProcessActors();
    void WorkerThread(workerID_t id);
     
    postOffice_t& postOffice;
    vectorOfWorkers_t workers;
    vectorOfInfo_t    infos;

    rwMutex_t          actorsGuard;
    actorStorage_t     actors;

    workerPool_t(postOffice_t& postOffice);
    ~workerPool_t();

    workerPool_t(const workerPool_t&) = delete;
    workerPool_t(workerPool_t&&) = delete;

    workerPool_t& operator = (const workerPool_t&) = delete;
    workerPool_t& operator = (workerPool_t&&) = delete;

    actorPID_t Register(std::unique_ptr<role_t>&& role);

  public:

    static workerPool_t& Instance();

    bool HasActorsInQueue();

    template<typename trole_t, typename... args_t>
    actorPID_t Register(args_t&&... args)
    {
      static_assert(std::is_base_of<bb::role_t, trole_t>::value,
        "Can be used only with bb::role_t subclasses"
      );
      return this->Register(
        std::unique_ptr<role_t>(new trole_t(std::forward<args_t>(args)...))
      );
    }

    actorPID_t FindFirstByName(const std::string& name);

    int PostMessage(actorPID_t  actorID, msg_t&& message);

    int Unregister(actorPID_t  actorID);

  };

} // namespace bb

#endif /* __BB_CORE_ACTOR_WORKER_HEADER__ */