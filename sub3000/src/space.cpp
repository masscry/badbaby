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
      player::Update(&this->player, static_cast<float>(SPACE_TIME_STEP));
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
  {
    bb::config_t config;
    config.Load("./level.config");

    int totalUnits = static_cast<int>(config.Value("level.gen.units", 10));
    float genRadius = static_cast<float>(config.Value("level.gen.radius", 1.0f));

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(-genRadius, genRadius);
    bb::linePoints_t unitPos;

    for (int i = 0; i < totalUnits; ++i)
    {
      glm::vec2 pos;

      this->speeds.emplace_back(
        0.0f
      );

      this->units.emplace_back(
        dist(mt),
        dist(mt)
      );
    }

    this->player.mass = static_cast<float>(config.Value("player.mass", 1.0f));
    this->player.rotMoment = static_cast<float>(config.Value("player.moment", 1.0f));
    
  }

  state_t::state_t(bb::vec2_t pos, float angle, const bb::linePoints_t& units)
  {
    glm::mat3 toLocal(1.0f);

    toLocal = glm::rotate(toLocal, angle);
    toLocal = glm::translate(toLocal, pos);
    toLocal = glm::scale(toLocal, glm::vec2(0.1f));

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

      bb::vec2_t force(0.0f);
      bb::vec2_t velDir(0.0f);

      float velLen = glm::dot(data->vel, data->vel);
      if (velLen != 0.0f)
      {
        velDir = glm::normalize(data->vel);
      }

      bb::vec2_t dir;

      sincosf(data->angle, &dir.x, &dir.y);

      force += dir * data->engineOutput - velDir*velLen/2.0f*0.8f;
      data->pos += data->vel * dt;
      data->vel += force/data->mass * dt;

      float rotVel = (data->rudderPos / data->rotMoment) * velLen;

      data->angle += rotVel * dt;

      // force angle in 0 - 2*PI
      data->angle = fmodf(
        data->angle,
        static_cast<float>(2.0*M_PI)
      );

      float expectedOutput = engine::Output(data->engine);

      data->engineOutput += glm::clamp(
        expectedOutput - data->engineOutput,
        -player::MAX_OUTPUT_CHANGE,
         player::MAX_OUTPUT_CHANGE
      )*dt;

      float expectedRudder = rudder::Output(data->rudder);

      data->rudderPos += glm::clamp(
        expectedRudder - data->rudderPos,
        -player::MAX_ANGLE_CHANGE,
         player::MAX_ANGLE_CHANGE
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
      return 1;
    }

  }

}