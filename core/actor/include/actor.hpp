/**
 * @file actor.hpp
 * 
 * Class implements abstract actor which can receive and send messages.
 * 
 */
#pragma once
#ifndef __BB_CORE_ACTOR_HEADER__
#define __BB_CORE_ACTOR_HEADER__

#include <msg.hpp>

namespace bb
{

  class actor_t
  {
    friend class workerPool_t;

    virtual void ProcessMessage(msg_t msg) = 0;

  public:

    actor_t() = default;
    actor_t(const actor_t& copy) = default;
    actor_t(actor_t&& copy) = default;
    actor_t& operator =(const actor_t& copy) = default;
    actor_t& operator =(actor_t&& copy) = default;
    virtual ~actor_t() = default;

  };

}

#endif /* __BB_CORE_ACTOR_HEADER__ */