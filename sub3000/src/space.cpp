#include <space.hpp>
#include <simplex.hpp>
#include <worker.hpp>
#include <msg.hpp>

#include <cassert>
#include <cmath>

#include <random>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

namespace sub3000
{

  const double SPACE_TIME_STEP = 1.0/30.0;

  void space_t::Step(double dt)
  {
    this->cumDT += dt;
    while (this->cumDT > SPACE_TIME_STEP)
    {
      auto unit = this->units.begin();
      auto speed = this->speeds.begin();
      for (int i = 0; i < 10; ++i)
      {
        *unit += *speed * static_cast<float>(SPACE_TIME_STEP);
        ++unit;
        ++speed;
      }
      this->cumDT -= SPACE_TIME_STEP;
    }
  }

  bb::msg::result_t space_t::OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg)
  {
    if (auto step = bb::msg::As<step_t>(msg))
    {
      this->Step(step->DeltaTime());

      if (step->Source() != bb::INVALID_ACTOR)
      {
        bb::workerPool_t::Instance().PostMessage(
          step->Source(),
          bb::Issue<state_t>(this->pos, this->units)
        );
      }
      return bb::msg::result_t::complete;
    }

    bb::Error("Unknown message: %s", typeid(msg).name());
    assert(0);
    return bb::msg::result_t::error;
  }

  space_t::space_t()
  {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    bb::linePoints_t unitPos;

    this->pos = bb::vec2_t(0.0f);
    this->dir = bb::vec2_t(0.0f, -1.0f);

    for (int i = 0; i < 10; ++i)
    {
      glm::vec2 pos;
      glm::vec2 speed;

      float angle = static_cast<float>(dist(mt)*M_PI*2.0f);
      float pspeed = 0.1f;

      this->speeds.emplace_back(
        pspeed*cos(angle),
        pspeed*sin(angle)
      );

      this->units.emplace_back(
        (dist(mt) - 0.5f)*2.0f,
        (dist(mt) - 0.5f)*2.0f
      );
    }
  }

}