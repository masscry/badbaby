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
#include <player.hpp>

namespace sub3000
{

  class state_t final: public bb::msg::basic_t
  {
    bb::linePoints_t units;
    std::vector<float> radarZ;
    player::data_t player;
  public:

    player::data_t& Player();

    const player::data_t& Player() const;

    bb::vec2_t& Vel();

    bb::vec2_t& Pos();
    float& Angle();
    float& Depth();
    float RadarAngle();

    bb::linePoints_t& Units();

    const bb::linePoints_t& Units() const;

    std::vector<float>& RadarZ();

    const std::vector<float>& RadarZ() const;

    state_t(bb::linePoints_t&& units, const std::vector<float>& radarZ, const player::data_t& player);

    state_t(const state_t&) = default;
    state_t& operator=(const state_t&) = default;

    state_t(state_t&&) = default;
    state_t& operator=(state_t&&) = default;

    ~state_t() override = default;
  };

  inline state_t::state_t(bb::linePoints_t&& units, const std::vector<float>& radarZ, const player::data_t& player)
  : units(std::move(units)),
    radarZ(radarZ),
    player(player)
  {
    ;
  }

  inline player::data_t& state_t::Player()
  {
    return this->player;
  }

  inline const player::data_t& state_t::Player() const
  {
    return this->player;
  }

  inline bb::vec2_t& state_t::Vel()
  {
    return this->player.vel;
  }

  inline float& state_t::Depth()
  {
    return this->player.depth;
  }

  inline bb::vec2_t& state_t::Pos()
  {
    return this->player.pos;
  }

  inline float& state_t::Angle()
  {
    return this->player.angle;
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

  inline float state_t::RadarAngle()
  {
    return this->player.RadarAngle();
  }

}

#endif /* __SUB3000_ARENA_STATE_HEADER__ */
