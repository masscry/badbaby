/**
 * @file actor.hpp
 * 
 * Class implements abstract actor which can receive and send messages.
 * 
 */
#pragma once
#ifndef __BB_CORE_ACTOR_HEADER__
#define __BB_CORE_ACTOR_HEADER__

#include <string>
#include <atomic>

#include <msg.hpp>

namespace bb
{

  class workerPool_t;

  class actor_t
  {
    friend class workerPool_t;

    std::string name;
    int id;
    workerPool_t* pool;

    virtual void OnProcessMessage(msg_t msg) = 0;

    void Unregister();

    void SetPoolID(workerPool_t* pool, int id)
    {
      this->pool = pool;
      this->id = id;
    }

  protected:

    // assume, than only actor descendents can post messages to pool as actor
    void PostMessageAsMe(int actorID, msg_t msg);

  public:

    void ProcessMessage(msg_t msg);

    int ID() const
    {
      return this->id;
    }

    const std::string& Name() const
    {
      return this->name;
    }

    void SetName(const std::string& name)
    {
      this->name = name;
    }

    actor_t();
    actor_t(const actor_t& src);
    actor_t(actor_t&& src);
    actor_t& operator =(const actor_t& src);
    actor_t& operator =(actor_t&& src);
    virtual ~actor_t();

  };

}

#endif /* __BB_CORE_ACTOR_HEADER__ */