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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

#include <control.hpp>
#include <player.hpp>
#include <mapGen.hpp>

#include <state_t.hpp>

namespace sub3000
{

  class step_t final: public bb::msg::basic_t
  {
    double dt;
  public:

    double DeltaTime() const;

    step_t(bb::actorPID_t src, double dt);

    step_t(const step_t&) = default;
    step_t& operator= (const step_t&) = default;

    step_t(step_t&&) = default;
    step_t& operator= (step_t&&) = default;

    ~step_t() override = default;
  };

  class space_t final: public bb::role_t
  {
    double cumDT;
    int newPointCount;
    bool renderDepth;
    player::data_t player;

    bb::linePoints_t radarXY;
    std::vector<float> radarZ;

    bb::ext::heightMap_t heightMap;
    bb::ext::distanceMap_t distMap;

    bb::msg::result_t OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg) override;

  public:

    void Step(double dt);

    const char* DefaultName() const override;

    space_t();
    ~space_t() override = default;

  };

  inline double step_t::DeltaTime() const
  {
    return this->dt;
  }

  inline step_t::step_t(bb::actorPID_t src, double dt)
  : bb::msg::basic_t(src),
    dt(dt)
  {
    ;
  }

  inline const char* space_t::DefaultName() const
  {
    return "space";
  }

} // namespace sub3000

#endif /* __SUB3000_SPACE_HEADER__ */