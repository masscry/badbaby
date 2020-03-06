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
      player::Update(&this->player, static_cast<float>(SPACE_TIME_STEP));
    }

    while (this->newPointDT > NEW_POINT_TIME)
    {
      this->newPointDT -= NEW_POINT_TIME;

      if (this->units.size() > 1000)
      {
        this->units.pop_front();
      }
      this->units.emplace_back(
        -this->player.pos
      );
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
  }

  state_t::state_t(bb::vec2_t pos, float angle, const bb::linePoints_t& units)
  {
    glm::mat3 toLocal(1.0f);

    toLocal = glm::rotate(toLocal, angle);
    toLocal = glm::translate(toLocal, pos);

    for (const auto& unit: units)
    {
      this->units.emplace_back(
        toLocal*glm::vec3(unit, 1.0f)
      );
    }
  }

  namespace player
  {

    void Update(data_t* data, float dt)
    {
      if (data == nullptr)
      { // programmer's mistake
        assert(0);
        return;
      }

      float velLen = glm::dot(data->vel, data->vel); // two times velocity
      bb::vec2_t velDir(0.0f);
      if (velLen != 0.0f)
      {
        velDir = glm::normalize(data->vel);
      }

      // shipDir - direction in which linear force applied
      bb::vec2_t shipDir;
      sincosf(data->angle, &shipDir.x, &shipDir.y);

      data->crossSection = static_cast<float>(M_PI) * data->width 
        * glm::mix(data->length, data->width, abs(glm::dot(velDir, shipDir)));
    
      data->dragCoeff = 0.5f * velLen * 0.8f * data->crossSection;

      // dragForce - force applied to stop ship in direction of it moving
      bb::vec2_t dragForce( // drag - two times speed in opposite direction of vel
        -velDir*data->dragCoeff // add config params!
      );

      // rudderDir.y - part of force applied to linear velocity
      // rudderDir.x - part of force applied to rotation
      bb::vec2_t rudderDir;
      sincosf(data->rudderPos, &rudderDir.x, &rudderDir.y);

      // linear force = engineForce + dragForce
      bb::vec2_t linForce = shipDir * data->engineOutput * rudderDir.y
        + dragForce;

      float aVelLen = data->aVel*data->aVel;
      float aVelDir = 0.0f;
      if (aVelLen != 0.0f)
      {
        aVelDir = data->aVel/abs(data->aVel);
      }

      float rotDragCoeff = 0.5f * aVelLen * 0.8f 
        * static_cast<float>(M_PI)*(data->width*data->width);

      // rotDrag - force applied to stop ship from rotating
      float rotDragForce = -aVelDir*rotDragCoeff;

      // water flow help to rotate ship if no force applied from engine
      // idea - projection of ship velocity on ship direction gives
      // value proportional to force which to add to engine output
      float rotFromLinVel = glm::dot(data->vel, shipDir);

      float rotForce = (data->engineOutput + rotFromLinVel*10.0f) * rudderDir.x
        + rotDragForce;

      data->pos += data->vel * dt;
      data->vel += linForce/data->mass * dt;

      data->angle += data->aVel * dt;
      data->aVel += rotForce/data->rotMoment * dt;

      // force angle in 0 - 2*PI
      data->angle = fmodf(
        data->angle,
        static_cast<float>(2.0*M_PI)
      );

      float expectedOutput = data->engineModeList.Output(data->engine);

      data->engineOutput += glm::clamp(
        expectedOutput - data->engineOutput,
        -data->maxOutputChange,
         data->maxOutputChange
      )*dt;

      float expectedRudder = rudder::Output(data->rudder);

      data->rudderPos += glm::clamp(
        expectedRudder - data->rudderPos,
        -data->maxAngleChange,
         data->maxAngleChange
      )*dt;
    }

    int Control(data_t* data, const bb::msg::keyEvent_t& key)
    {
      if (data == nullptr)
      {
        assert(0);
        return -1;
      }

      if (key.Press() == GLFW_RELEASE)
      {
        return 0;
      }

      int control = (key.Key() == GLFW_KEY_DOWN) - (key.Key() == GLFW_KEY_UP);
      int newOutput = control + data->engine;
      if ((newOutput >= engine::full_ahead) && (newOutput <= engine::full_astern))
      {
        data->engine = static_cast<engine::mode_t>(newOutput);
      }

      int rotate = (key.Key() == GLFW_KEY_RIGHT) - (key.Key() == GLFW_KEY_LEFT);
      int newRudder = rotate + data->rudder;
      if ((newRudder >= rudder::left_40) && (newRudder <= rudder::right_40))
      {
        data->rudder = static_cast<rudder::mode_t>(newRudder);
      }
      if (key.Key() == GLFW_KEY_ESCAPE)
      {
        sub3000::PostChangeScene(sub3000::sceneID_t::mainMenu);
      }
      return 1;
    }

  }

}