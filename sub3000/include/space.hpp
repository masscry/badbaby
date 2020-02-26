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
    double dt;
  public:

    double DeltaTime() const
    {
      return this->dt;
    }

    step_t(int src, double dt)
    : bb::msg::basic_t(src),
      dt(dt)
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

    state_t(bb::vec2_t pos, const bb::linePoints_t& units)
    {
      this->units.assign(units.begin(), units.end());
      for (auto& unit: this->units)
      {
        unit -= pos;
      }
    }

    state_t(const state_t&) = default;
    state_t& operator=(const state_t&) = default;

    state_t(state_t&&) = default;
    state_t& operator=(state_t&&) = default;

    ~state_t() override = default;
  };

  class space_t final: public bb::role_t
  {
    double cumDT;
    bb::vec2_t pos;
    bb::vec2_t dir;

    bb::linePoints_t units;
    bb::linePoints_t speeds;

    bb::msg::result_t OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg) override;

  public:

    void Step(double dt);

    const char* DefaultName() const override;

    space_t();
    ~space_t() override = default;

  };

  inline const char* space_t::DefaultName() const
  {
    return "space";
  }

} // namespace sub3000

#endif /* __SUB3000_SPACE_HEADER__ */