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

  const double NEW_POINT_TIME = 0.02;

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

      if (this->distMap.IsGood())
      {
        if (this->units.size() >= 720)
        {
          this->units.pop_front();
        }

        auto start = glfwGetTime();
        auto dir = bb::Dir(glm::radians(this->player.radarAngle)-this->player.angle);

        bb::vec3_t isec;

        if (this->distMap.CastRay(
            bb::vec3_t(this->player.pos - bb::vec2_t(0.5f), this->player.depth),
            bb::vec3_t(dir, 0.0f),
            &isec, -1.0f
            )
          )
        {
          this->units.emplace_back(bb::vec2_t(isec) + bb::vec2_t(0.5f));
        }

        this->player.radarAngle = fmodf(this->player.radarAngle + 2.0f, 359.0f);

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
          bb::Issue<state_t>(
            this->player.pos,
            this->player.angle,
            this->player.depth,
            std::move(this->units),
            this->player.radarAngle,
            this->player.vel
          )
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

    if (auto mapReady = bb::msg::As<bb::ext::hmDone_t>(msg))
    {
      this->heightMap = mapReady->HeightMap();
      this->distMap = mapReady->DistanceMap();

      if (!bb::ext::binstore_t::Read("world.bbw").IsGood())
      {
        auto worldWriter = bb::ext::binstore_t::Create("world.bbw");
        this->distMap.Serialize(worldWriter);
      }

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
