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

  void space_t::Step()
  {
    auto unit = this->units.begin();
    auto speed = this->speeds.begin();
    for (int i = 0; i < 10; ++i)
    {
      *unit += *speed;
      ++unit;
      ++speed;
    }
  }

  bb::msg::result_t space_t::OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg)
  {
    if (auto step = bb::msg::As<step_t>(msg))
    {
      for (int i = 0; i < step->Count(); ++i)
      {
        this->Step();
      }

      if (step->Source() != bb::INVALID_ACTOR)
      {
        bb::workerPool_t::Instance().PostMessage(
          step->Source(),
          bb::Issue<state_t>(this->units)
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