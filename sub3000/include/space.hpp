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

  enum engineMode_t
  {
    EM_FULL_AHEAD = 0,
    EM_HALF_AHEAD,
    EM_SLOW_AHEAD,
    EM_DEAD_SLOW_AHEAD,
    EM_STOP,
    EM_DEAD_SLOW_ASTERN,
    EM_SLOW_ASTERN,
    EM_HALF_ASTERN,
    EM_FULL_ASTERN,
    EM_TOTAL
  };

  inline const char* EngineModeString(engineMode_t engineMode)
  {
    switch (engineMode)
    {
      case EM_FULL_AHEAD:
        return "Full Ahead";
      case EM_HALF_AHEAD:
        return "Half Ahead";
      case EM_SLOW_AHEAD:
        return "Slow Ahead";
      case EM_DEAD_SLOW_AHEAD:
        return "Dead Slow Ahead";
      case EM_STOP:
        return "Stop";
      case EM_DEAD_SLOW_ASTERN:
        return "Dead Slow Astern";
      case EM_SLOW_ASTERN:
        return "Slow Astern";
      case EM_HALF_ASTERN:
        return "Half Astern";
      case EM_FULL_ASTERN:
        return "Full Astern";
      default:
        assert(0);
        return "???";
    }
  }

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
        unit = (unit - pos)*0.2f;
      }
    }

    state_t(const state_t&) = default;
    state_t& operator=(const state_t&) = default;

    state_t(state_t&&) = default;
    state_t& operator=(state_t&&) = default;

    ~state_t() override = default;
  };

  class playerStatus_t: public bb::msg::basic_t
  {
  public:

    bb::vec2_t pos;
    bb::vec2_t vel;

    playerStatus_t(bb::vec2_t pos, bb::vec2_t vel)
    : pos(pos),
      vel(vel)
    {
      ;
    }

    playerStatus_t(const playerStatus_t&) = default;
    playerStatus_t& operator=(const playerStatus_t&) = default;
    playerStatus_t(playerStatus_t&&) = default;
    playerStatus_t& operator=(playerStatus_t&&) = default;
    ~playerStatus_t() override = default;

  };

  class control_t: public bb::msg::basic_t
  {
    int control;
  public:

    int Control() const
    {
      return this->control;
    }

    control_t(int control)
    : control(control)
    {
      ;
    }

    control_t(const control_t&) = default;
    control_t& operator=(const control_t&) = default;
    control_t(control_t&&) = default;
    control_t& operator=(control_t&&) = default;
    ~control_t() override = default;
  };

  inline float EngineOutput(engineMode_t engineMode)
  {
    switch(engineMode)
    {
    case EM_FULL_AHEAD:
      return 1.0f;
    case EM_HALF_AHEAD:
      return 0.5f;
    case EM_SLOW_AHEAD:
      return 0.25f;
    case EM_DEAD_SLOW_AHEAD:
      return 0.1f;
    case EM_STOP:
      return 0.0f;
    case EM_DEAD_SLOW_ASTERN:
      return -0.05f;
    case EM_SLOW_ASTERN:
      return -0.1f;
    case EM_HALF_ASTERN:
      return -0.2f;
    case EM_FULL_ASTERN:
      return -0.4f;
    default:
      // Programmer's mistake!
      assert(0);
      return 0.0f;
    }
  }

  struct player_t final
  {
    bb::vec2_t pos;
    bb::vec2_t vel;
    bb::vec2_t dir;
    engineMode_t engineMode;

    void Update(float dt)
    {
      float engineOutput = EngineOutput(this->engineMode);
      bb::vec2_t force(0.0f);
      bb::vec2_t velDir(0.0f);
      float velLen = glm::dot(this->vel, this->vel);
      if (velLen != 0.0f)
      {
        velDir = glm::normalize(this->vel);
      }

      force += dir * engineOutput - velDir*velLen/2.0f*0.8f;

      this->pos += this->vel * dt;
      this->vel += force * dt;
    }

    player_t()
    : pos(0.0f), vel(0.0f), dir(0.0f, 1.0f), engineMode(EM_STOP)
    {
      ;
    }

    player_t(const player_t&) = default;
    player_t& operator=(const player_t&) = default;
    player_t(player_t&&) = default;
    player_t& operator=(player_t&&) = default;
    ~player_t() = default;
  };

  class space_t final: public bb::role_t
  {
    double cumDT;
    player_t player;

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