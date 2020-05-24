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
    std::vector<float> radarZ;
    bb::vec2_t pos;
    bb::vec2_t vel;
    float angle;
    float depth;
    float radarAngle;
  public:

    bb::vec2_t& Vel();

    bb::vec2_t& Pos();
    float& Angle();
    float& Depth();
    float& RadarAngle();

    bb::linePoints_t& Units();

    const bb::linePoints_t& Units() const;

    std::vector<float>& RadarZ();

    const std::vector<float>& RadarZ() const;

    state_t(bb::vec2_t pos, float angle, float depth, bb::linePoints_t&& units, float radarAngle, bb::vec2_t vel, const std::vector<float>& radarZ);

    state_t(const state_t&) = default;
    state_t& operator=(const state_t&) = default;

    state_t(state_t&&) = default;
    state_t& operator=(state_t&&) = default;

    ~state_t() override = default;
  };

  inline state_t::state_t(bb::vec2_t pos, float angle, float depth, bb::linePoints_t&& units, float radarAngle, bb::vec2_t vel, const std::vector<float>& radarZ)
  : units(std::move(units)),
    radarZ(radarZ),
    pos(pos),
    vel(vel),
    angle(angle),
    depth(depth),
    radarAngle(radarAngle)
  {
    ;
  }

  inline bb::vec2_t& state_t::Vel()
  {
    return this->vel;
  }

  inline float& state_t::Depth()
  {
    return this->depth;
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

  inline std::vector<float>& state_t::RadarZ()
  {
    return this->radarZ;
  }

  inline const std::vector<float>& state_t::RadarZ() const
  {
    return this->radarZ;
  }

  inline float& state_t::RadarAngle()
  {
    return this->radarAngle;
  }

}

#endif /* __SUB3000_ARENA_STATE_HEADER__ */
