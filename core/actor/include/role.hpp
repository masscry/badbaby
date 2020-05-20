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
#include <string>

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

  class execTask_t: public role_t
  {
    std::string name;

    msg::result_t OnProcessMessage(const actor_t& self, const msg::basic_t& msg) override;

  public:

    const char* DefaultName() const override;

    execTask_t();
    ~execTask_t() override = default;
  };

  inline msg::result_t execTask_t::OnProcessMessage(const actor_t&, const msg::basic_t& msg)
  {
    if (auto execTask = bb::msg::As<msg::basicExecTask_t>(msg))
    {
      return execTask->Execute();
    }
    return msg::result_t::error;
  }

  inline const char* execTask_t::DefaultName() const
  {
    return this->name.c_str();
  }

  inline execTask_t::execTask_t()
  : name(bb::GenerateUniqueName())
  {
    ;
  }

} // namespace bb

#endif /* __BB_CORE_ACTOR_ROLE_HEADER__ */