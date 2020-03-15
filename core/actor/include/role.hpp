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
#include <memory>
#include <type_traits>

namespace bb
{

  class role_t
  {  
  private:

    virtual msg::result_t OnProcessMessage(const actor_t& self, const msg::basic_t& msg) = 0;

  public:

    msg::result_t ProcessMessage(const actor_t& self, const msg::basic_t& msg);

    virtual const char* DefaultName() const;

    virtual ~role_t();

  };

  inline msg::result_t role_t::ProcessMessage(const actor_t& self, const msg::basic_t& msg)
  {
    return this->OnProcessMessage(self, msg);
  }

  inline const char* role_t::DefaultName() const
  {
    return "???";
  }

  using uniqueRole_t = std::unique_ptr<role_t>;

} // namespace bb

#endif /* __BB_CORE_ACTOR_ROLE_HEADER__ */