/**
 * @file role.hpp
 * 
 * Role - user-implemented behavior for actor
 * 
 */

#pragma once
#ifndef __BB_CORE_ACTOR_ROLE_HEADER__
#define __BB_CORE_ACTOR_ROLE_HEADER__

#include <msg.hpp>
#include <actor.hpp>

namespace bb
{

  class role_t
  {  
  private:

    virtual msgResult_t OnProcessMessage(const actor_t& self, msg_t msg) = 0;

  public:

    msgResult_t ProcessMessage(const actor_t& self, msg_t msg);

    virtual const char* DefaultName() const;

    virtual ~role_t();

  };

  inline msgResult_t role_t::ProcessMessage(const actor_t& self, msg_t msg)
  {
    return this->OnProcessMessage(self, msg);
  }

  inline const char* role_t::DefaultName() const
  {
    return "???";
  }

} // namespace bb

#endif /* __BB_CORE_ACTOR_ROLE_HEADER__ */