/**
 * @file space.hpp
 * 
 * Tactical world simulation actor
 * 
 */

#pragma once
#ifndef __SUB3000_SPACE_HEADER__
#define __SUB3000_SPACE_HEADER__

#include <actor.hpp>
#include <role.hpp>

namespace sub3000
{

  class step_t final: public bb::msg::basic_t
  {
  public:
    step_t() { ; }
    ~step_t() override = default;
  };

  class space_t final: public bb::role_t
  {

    bb::msg::result_t OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg) override;

  public:

    const char* DefaultName() const override;

    space_t();
    ~space_t() override = default;

  };

  const char* space_t::DefaultName() const
  {
    return "space";
  }

} // namespace sub3000

#endif /* __SUB3000_SPACE_HEADER__ */