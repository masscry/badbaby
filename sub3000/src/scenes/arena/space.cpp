#include <space.hpp>
#include <simplex.hpp>
#include <worker.hpp>
#include <msg.hpp>
#include <context.hpp>

#include <cassert>
#include <cmath>

#include <random>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

#include <scene.hpp>

#include <mapGen.hpp>

namespace sub3000
{

  namespace engine 
  {
  
    modeList_t::modeList_t(const bb::config_t& config)
    {
      this->output[mode_t::full_ahead] = static_cast<float>(config.Value("engine.full_ahead", 0.5));
      this->output[mode_t::half_ahead] = static_cast<float>(config.Value("engine.half_ahead", 0.25));
      this->output[mode_t::slow_ahead] = static_cast<float>(config.Value("engine.slow_ahead", 0.125));
      this->output[mode_t::dead_slow_ahead] = static_cast<float>(config.Value("engine.dead_slow_ahead", 0.05));
      this->output[mode_t::stop] = 0.0f;
      this->output[mode_t::dead_slow_astern] = static_cast<float>(config.Value("engine.slow_astern", -0.025));
      this->output[mode_t::slow_astern] = static_cast<float>(config.Value("engine.slow_astern", -0.05));
      this->output[mode_t::half_astern] = static_cast<float>(config.Value("engine.half_astern", -0.125));
      this->output[mode_t::full_astern] = static_cast<float>(config.Value("engine.full_astern", -0.25));
    }

  } // namespace engine


  const double SPACE_TIME_STEP = 1.0/30.0;

  const double NEW_POINT_TIME = 1.0;

  void space_t::Step(double dt)
  {
    this->cumDT += dt;
    this->newPointDT += dt;
    while (this->cumDT > SPACE_TIME_STEP)
    {
      this->cumDT -= SPACE_TIME_STEP;
      player::Update(&this->player, this->heightMap, static_cast<float>(SPACE_TIME_STEP));
    }

    while (this->newPointDT > NEW_POINT_TIME)
    {
      this->newPointDT -= NEW_POINT_TIME;

      if (this->heightMap.IsGood())
      {
        this->units.clear();

        auto start = glfwGetTime();
        for (float angle = -180.0f, finish = 180.0f; angle < finish; angle += 1.0f )
        {
          bb::vec2_t isec;

          for (float height = 0.4f; height <= 0.50f; height += 0.01)
          {
            if (this->heightMap.RayCast(this->player.pos, bb::Dir(this->player.angle + angle), height, 1.0f, 10.0f, &isec))
            {
              this->units.emplace_back(
                isec
              );
            }
          }
        }
        auto finish = glfwGetTime();
        bb::context_t::Instance().Title(
          std::to_string(
            finish - start
          )
        );
      }
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
          bb::Issue<state_t>(this->player.pos, this->player.angle, this->units)
        );

        bb::postOffice_t::Instance().Post(
          "arenaStatus",
          bb::Issue<player::status_t>(
            this->player
          )
        );
      }
      return bb::msg::result_t::complete;
    }

    if (auto key = bb::msg::As<bb::msg::keyEvent_t>(msg))
    {
      player::Control(&this->player, *key);
      return bb::msg::result_t::complete;
    }

    if (auto mapReady = bb::msg::As<bb::ext::done_t>(msg))
    {
      this->heightMap = mapReady->HeightMap();

      bb::postOffice_t::Instance().Post(
        "arenaScreen",
        bb::Issue<bb::ext::done_t>(*mapReady)
      );
      return bb::msg::result_t::complete;
    }

    bb::Error("Unknown message: %s", typeid(msg).name());
    assert(0);
    return bb::msg::result_t::error;
  }

  space_t::space_t()
  : cumDT(0.0),
    newPointDT(0.0)
  {
    bb::config_t config;
    config.Load("./arena.config");

    this->player.mass = static_cast<float>(config.Value("player.mass", 1.0f));
    this->player.rotMoment = static_cast<float>(config.Value("player.moment", 1.0f));
    this->player.engineModeList = engine::modeList_t(config);
    this->player.maxOutputChange =  static_cast<float>(config.Value("player.max.change.output", 0.1f));
    this->player.maxAngleChange =  static_cast<float>(config.Value("player.max.change.angle", 0.3f));
    this->player.width = static_cast<float>(config.Value("player.width", 1.0f));
    this->player.length =  static_cast<float>(config.Value("player.length", 1.0f));

    this->player.pos = glm::vec2(240.0f, 117.0f);
    this->player.angle = 0.0f;
  }

}
