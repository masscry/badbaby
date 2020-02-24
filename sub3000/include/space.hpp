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
#include <meshDesc.hpp>

namespace sub3000
{

  class step_t final: public bb::msg::basic_t
  {
    int count;
  public:

    int Count() const
    {
      return this->count;
    }

    step_t(int count)
    : count(count)
    { 
      ;
    }

    step_t(const step_t&) = default;
    step_t& operator= (const step_t&) = default;

    step_t(step_t&&) = default;
    step_t& operator= (step_t&&) = default;

    ~step_t() override = default;
  };

  class state_t final: public bb::msg::basic_t
  {
    bb::linePoints_t units;
  public:

    bb::linePoints_t& Units()
    {
      return this->units;
    }

    const bb::linePoints_t& Units() const
    {
      return this->units;
    }

    state_t()
    {
      ;
    }

    state_t(const bb::linePoints_t& units)
    : units(units)
    {
      ;
    }

    state_t(const state_t&) = default;
    state_t& operator=(const state_t&) = default;

    state_t(state_t&&) = default;
    state_t& operator=(state_t&&) = default;

    ~state_t() override = default;
  };

  class space_t final: public bb::role_t
  {
    bb::linePoints_t units;
    bb::linePoints_t speeds;

    bb::msg::result_t OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg) override;

  public:

    void Step();

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