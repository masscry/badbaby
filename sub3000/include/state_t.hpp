/**
 * @file state_t.hpp
 * 
 * World state
 */
#pragma once
#ifndef __SUB3000_ARENA_STATE_HEADER__
#define __SUB3000_ARENA_STATE_HEADER__

#include <msg.hpp>
#include <algebra.hpp>
#include <meshDesc.hpp>

namespace sub3000
{

  class state_t final: public bb::msg::basic_t
  {
    bb::linePoints_t units;
    bb::vec2_t pos;
    float angle;
  public:

    bb::vec2_t& Pos();
    float& Angle();

    bb::linePoints_t& Units();

    const bb::linePoints_t& Units() const;

    state_t(bb::vec2_t pos, float angle, const bb::linePoints_t& units);

    state_t(const state_t&) = default;
    state_t& operator=(const state_t&) = default;

    state_t(state_t&&) = default;
    state_t& operator=(state_t&&) = default;

    ~state_t() override = default;
  };

    inline state_t::state_t(bb::vec2_t pos, float angle, const bb::linePoints_t& units)
  : units(units),
    pos(pos),
    angle(angle)
  {
    ;
  }

  inline bb::vec2_t& state_t::Pos()
  {
    return this->pos;
  }

  inline float& state_t::Angle()
  {
    return this->angle;
  }

  inline bb::linePoints_t& state_t::Units()
  {
    return this->units;
  }

  inline const bb::linePoints_t& state_t::Units() const
  {
    return this->units;
  }

}

#endif /* __SUB3000_ARENA_STATE_HEADER__ */
