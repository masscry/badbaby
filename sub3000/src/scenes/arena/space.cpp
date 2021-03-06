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
      this->output[mode_t::dead_slow_astern] = static_cast<float>(config.Value("engine.dead_slow_astern", -0.025));
      this->output[mode_t::slow_astern] = static_cast<float>(config.Value("engine.slow_astern", -0.05));
      this->output[mode_t::half_astern] = static_cast<float>(config.Value("engine.half_astern", -0.125));
      this->output[mode_t::full_astern] = static_cast<float>(config.Value("engine.full_astern", -0.25));
    }

  } // namespace engine


  const double SPACE_TIME_STEP = 1.0/30.0;

  void space_t::Step(double dt)
  {
    if (!this->distMap.IsGood())
    {
      // while map is not ready, 
      // time do not advance
      return;
    }

    this->cumDT += dt;
    while (this->cumDT > SPACE_TIME_STEP)
    {
      this->cumDT -= SPACE_TIME_STEP;
      this->newPointCount += 10*this->simSpeed;
      this->renderDepth = true;

      FILE* output = fopen("ship.txt", "at");
      BB_DEFER(if (output != nullptr) { fclose(output); });

      for (int i = 0; i < this->simSpeed; ++i)
      {
        player::Update(&this->player, this->heightMap, static_cast<float>(SPACE_TIME_STEP));
        if (output != nullptr)
        {
          this->player.Dump(output);
        }
      }
    }

    if (this->renderDepth)
    {
      auto dir = bb::Dir(glm::radians(180.0f) - this->player.angle)*0.5f;
      auto cursor = this->player.pos - bb::vec2_t(0.5f) - dir*10.0f;

      for (auto& zDepth: this->radarZ)
      {
        zDepth = this->distMap.SampleHeightMap(bb::vec3_t(cursor + dir, this->player.depth)) / this->distMap.Depth();
        zDepth = std::max(0.0f, zDepth);
        cursor += dir;
      }
      this->renderDepth = false;

      bb::postOffice_t::Instance().Post(
        "arenaStatus",
        bb::Issue<player::status_t>(
          this->player
        )
      );
    }

    while (this->newPointCount-->0)
    {
      if (this->radarXY.size() >= 720)
      {
        this->radarXY.pop_front();
      }

      auto dir = bb::Dir(glm::radians(this->player.RadarAngle())-this->player.angle);

      bb::vec3_t isec;

      if (this->distMap.CastRay(
          bb::vec3_t(this->player.pos - bb::vec2_t(0.5f), this->player.depth),
          glm::normalize(bb::vec3_t(dir, 0.0f)),
          &isec, 10.0f
        )
      )
      {
        this->radarXY.emplace_back(bb::vec2_t(isec) + bb::vec2_t(0.5f));
      }

      this->player.radarAngle += this->player.radarAngleDelta;
      switch(this->player.radar)
      {
        case radar::radius360:
          this->player.radarAngle %= bb::deci_t(360);
          break;
        case radar::front90:
          if (this->player.radarAngle > 225)
          {
            this->player.radarAngleDelta = -1;
          }
          if (this->player.radarAngle < 135)
          {
            this->player.radarAngleDelta = +1;
          }
          break;
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
            std::move(this->radarXY),
            this->radarZ,
            this->player,
            this->simSpeed
          )
        );
      }
      return bb::msg::result_t::complete;
    }

    if (auto key = bb::msg::As<bb::msg::keyEvent_t>(msg))
    {
      if (key->Press() != GLFW_RELEASE)
      {
        int changeSimSpeed = (key->Key() == GLFW_KEY_RIGHT_BRACKET) - (key->Key() == GLFW_KEY_LEFT_BRACKET);
        if (changeSimSpeed != 0)
        {
          int newSimSpeed = this->simSpeed + changeSimSpeed;
          if ((newSimSpeed >= 1) && (newSimSpeed <= 4))
          {
            this->simSpeed = newSimSpeed;
          }
          return bb::msg::result_t::complete;
        }

        if (key->Key() == GLFW_KEY_F1)
        {
          this->simSpeed = 1;
          return bb::msg::result_t::complete;
        }
        if (key->Key() == GLFW_KEY_ESCAPE)
        {
          sub3000::PostChangeScene(sub3000::sceneID_t::mainMenu);
          return bb::msg::result_t::complete;
        }
      }
      if (this->simSpeed == 1)
      {
        player::Control(&this->player, *key);
      }
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

      bb::postOffice_t::Instance().Post(
        "arenaStatus",
        bb::Issue<bb::msg::dataMsg_t<bb::ext::heightMap_t>>(
          mapReady->HeightMap(),
          -1
        )
      );

      return bb::msg::result_t::complete;
    }

    bb::Error("Unknown message: %s", typeid(msg).name());
    assert(0);
    return bb::msg::result_t::error;
  }

  space_t::space_t()
  : cumDT(0.0),
    newPointCount(0),
    renderDepth(false)
  {
    bb::config_t config;
    config.Load("./arena.config");

    this->player.mass = static_cast<float>(config.Value("player.mass", 1.0f));
    this->player.rotMoment = static_cast<float>(config.Value("player.moment", 1.0f));
    this->player.engineModeList = engine::modeList_t(config);
    this->player.maxOutputChange =  static_cast<float>(config.Value("player.max.change.output", 0.1f));
    this->player.maxAngleChange =  static_cast<float>(config.Value("player.max.change.angle", 0.3f));
    this->player.maxBallastChange = static_cast<float>(config.Value("player.max.change.ballast", 0.6f));
    this->player.width = static_cast<float>(config.Value("player.width", 1.0f));
    this->player.length =  static_cast<float>(config.Value("player.length", 1.0f));

    this->player.pos.x = static_cast<float>(config.Value("player.pos.x", 240.0f));
    this->player.pos.y = static_cast<float>(config.Value("player.pos.y", 117.0f));
    this->player.angle = 0.0f;
    this->player.clip = (config.Value("clip", 1.0f) != 0.0f);

    this->radarZ.resize(20, 0.0f);

    this->simSpeed = 1;

    if (FILE* output = fopen("ship.txt", "wt"))
    {
      BB_DEFER(fclose(output));
      fprintf(output, "%s\n", "=== Sim Start ===");
      fprintf(output, "%s\n",
        "         POSITION            \t          VELOCITY           \t    ANGLE    \t  ANGLE VEL  \t    OUTPUT   \t   RUDDER    \t CROSSSECTION\t    DEPTH    "
      );
    }
  }

  space_t::~space_t()
  {
    if (FILE* output = fopen("ship.txt", "at"))
    {
      BB_DEFER(fclose(output));
      fprintf(output, "%s\n", "=== Sim Finish ===");
    }
  }

}
