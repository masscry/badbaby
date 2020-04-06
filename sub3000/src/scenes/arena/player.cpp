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

    bb::vec2_t ControlDir()
    {
      auto& context = bb::context_t::Instance();
      bb::vec2_t result = {
        context.IsKeyDown(GLFW_KEY_D) - context.IsKeyDown(GLFW_KEY_A),
        context.IsKeyDown(GLFW_KEY_S) - context.IsKeyDown(GLFW_KEY_W)
      };

      auto len = glm::length(result);
      if (len != 0.0f)
      {
        return result/len;
      }
      return bb::vec2_t(0.0f);
    }

    void Update(data_t* data, const bb::ext::heightMap_t&, float dt)
    {
      if (data == nullptr)
      { // programmer's mistake
        assert(0);
        return;
      }

      data->pos += ControlDir()*dt*10.0f;

      /*

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
      bb::vec2_t rudderDir = bb::Dir(data->rudderPos);

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

      */

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

      /*

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

      */

      if (key.Key() == GLFW_KEY_ESCAPE)
      {
        sub3000::PostChangeScene(sub3000::sceneID_t::mainMenu);
      }
      return 1;
    }

  }

}
