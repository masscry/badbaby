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
  
  namespace player
  {

    void data_t::Dump(FILE* output) const
    {
      fprintf(
        output,
        "[%+e;%+e]\t[%+e;%+e]\t%+e\t%+e\t%+e\t%+e\t%+e\t%+e\n",
        pos.x, pos.y,
        vel.x, vel.y,
        angle,
        aVel,
        engineOutput,
        rudderPos,
        crossSection,
        depth
      );
    }

    float ControlVal(uint16_t left, uint16_t right)
    {
      auto& context = bb::context_t::Instance();
      return context.IsKeyDown(left) - context.IsKeyDown(right);
    }

    bb::vec2_t ControlDir(uint16_t left, uint16_t right, uint16_t down, uint16_t up)
    {
      auto& context = bb::context_t::Instance();
      bb::vec2_t result = {
        context.IsKeyDown(left) - context.IsKeyDown(right),
        context.IsKeyDown(down) - context.IsKeyDown(up)
      };

      auto len = glm::length(result);
      if (len != 0.0f)
      {
        return result/len;
      }
      return bb::vec2_t(0.0f);
    }

    void Update(data_t* data, const bb::ext::heightMap_t& hmap, float dt)
    {
      if ((data == nullptr) || (!std::isfinite(dt)))
      { // programmer's mistake
        assert(0);
        return;
      }

      if (data->clip)
      {
        data->depth += ControlVal(GLFW_KEY_KP_ADD, GLFW_KEY_KP_SUBTRACT)*dt*10.0f;

        data->angle += ControlVal(GLFW_KEY_Q, GLFW_KEY_E)*dt;
        auto cdir = ControlDir(GLFW_KEY_RIGHT, GLFW_KEY_LEFT, GLFW_KEY_DOWN, GLFW_KEY_UP);

        auto dir = -data->Dir();
        auto side = bb::vec2_t(dir.y, -dir.x);

        auto moveDir = dir*cdir.y + side*cdir.x;
        auto moveDirLen = glm::length(moveDir);
        if (glm::length(moveDir) != 0.0f)
        {
          moveDir /= moveDirLen;
        }

        data->pos += moveDir*dt*10.0f;
      }
      else
      {
        float velLen = glm::dot(data->vel, data->vel); // two times velocity
        bb::vec2_t velDir(0.0f);
        if (velLen != 0.0f)
        {
          velDir = glm::normalize(data->vel);
        }

        // shipDir - direction in which linear force applied
        auto shipDir = data->Dir();

        data->crossSection = static_cast<float>(M_PI) * data->width 
          * glm::mix(data->length, data->width, std::fabs(glm::dot(velDir, shipDir)));
      
        data->dragCoeff = 0.5f * velLen * 0.8f * data->crossSection;

        // dragForce - force applied to stop ship in direction of it moving
        bb::vec2_t dragForce( // drag - two times speed in opposite direction of vel
          -velDir*data->dragCoeff // add config params!
        );

        // rudderDir.y - part of force applied to linear velocity
        // rudderDir.x - part of force applied to rotation
        bb::vec2_t rudderDir = bb::Dir(-data->rudderPos);

        // linear force = engineForce + dragForce
        bb::vec2_t linForce = shipDir * data->engineOutput * rudderDir.y 
          + dragForce;

        float aVelLen = data->aVel*data->aVel;
        float aVelDir = 0.0f;
        if (aVelLen != 0.0f)
        {
          aVelDir = data->aVel/std::fabs(data->aVel);
        }

        float rotDragCoeff = 0.5f * aVelLen * 0.8f 
          * static_cast<float>(M_PI)*(data->length*data->length);

        // rotDrag - force applied to stop ship from rotating
        float rotDragForce = -aVelDir*rotDragCoeff;

        // water flow help to rotate ship if no force applied from engine
        // idea - projection of ship velocity on ship direction gives
        // value proportional to force which to add to engine output
        float rotFromLinVel = glm::dot(data->vel, shipDir);

        float rotForce = (data->engineOutput + rotFromLinVel*10.0f) * rudderDir.x
          + rotDragForce;

        auto newPos = data->pos + data->vel * dt;
        auto newAngle = data->angle + data->aVel * dt;

        if (hmap.Sample(newPos - glm::vec2(0.5f))*63.0f > data->depth - data->width*0.2f)
        {
          auto velLen = glm::length(data->vel);
          auto normal = glm::vec2(hmap.NormalAtPoint(newPos - glm::vec2(0.5f), 63.0f));
          auto normLen = glm::length(normal);

          if ((velLen != 0.0f) && (normLen != 0.0f))
          {
            auto normalVel = data->vel/velLen;
            auto normal2D = normal/normLen;

            auto theta = atan2f(
              glm::dot(glm::vec2(normal2D.y, -normal2D.x) , normalVel),
              glm::dot(normal2D, normalVel)
            );

            assert(std::isfinite(normal2D.x) && std::isfinite(normal2D.y));
            assert(std::isfinite(theta));

            linForce += normal2D * velLen*100.0f;
            rotForce += theta * 100.0f;
          }
        }

        assert(std::isfinite(linForce.x) && std::isfinite(linForce.y));
        assert(std::isfinite(newPos.x) && std::isfinite(newPos.y));
        assert(std::isfinite(rotForce));
        assert(std::isfinite(newAngle));

        data->pos = newPos;
        data->vel += linForce/data->mass * dt;

        data->angle = newAngle;
        data->aVel += rotForce/data->rotMoment * dt;

        // force angle in 0 - 2*PI
        data->angle = fmodf(
          data->angle,
          static_cast<float>(2.0*M_PI)
        );

        float maxHeight = data->depth;
        float minHeight = data->depth;

        if (data->ballastStatus < 0.0f)
        {
          minHeight = hmap.Sample(data->pos)*63.0f + data->width*0.2f;
        }

        if (data->ballastStatus > 0.0f)
        {
          maxHeight = 28.0f;
        }

        data->depth = glm::clamp(
          data->depth + data->ballastStatus*dt,
          minHeight, maxHeight
        );

        float expectedOutput = data->engineModeList.Output(data->engine);

        data->engineOutput += glm::clamp(
          expectedOutput - data->engineOutput,
          -data->maxOutputChange,
          +data->maxOutputChange
        )*dt;

        float expectedRudder = rudder::Output(data->rudder);

        data->rudderPos += glm::clamp(
          expectedRudder - data->rudderPos,
          -data->maxAngleChange,
          +data->maxAngleChange
        )*dt;

        float expectedBallast = ballast::Output(data->ballast);

        data->ballastStatus += glm::clamp(
          expectedBallast - data->ballastStatus,
          -data->maxBallastChange,
          +data->maxBallastChange
        )*dt;
      }

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

      if (key.Key() == GLFW_KEY_C)
      {
        data->clip = !data->clip;
      }

      if (key.Key() == GLFW_KEY_F3)
      {
        data->radar = radar::front90;
      }
      if (key.Key() == GLFW_KEY_F4)
      {
        data->radar = radar::radius360;
        data->radarAngleDelta = bb::deci_t(1);
      }

      if (data->clip == false)
      {
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

        int bal = (key.Key() == GLFW_KEY_KP_ADD) - (key.Key() == GLFW_KEY_KP_SUBTRACT);
        int newBal = bal + data->ballast;
        if ((newBal >= ballast::blow) && (newBal <= ballast::pump))
        {
          data->ballast = static_cast<ballast::mode_t>(newBal);
        }
      }
      return 1;
    }

  }

}
