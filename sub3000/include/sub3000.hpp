/**
 * @file sub3000.hpp
 * @author timur
 * 
 * Globals and entry point
 * 
 */

#pragma once
#ifndef __SUB3000_HEADER__
#define __SUB3000_HEADER__

#include <GLFW/glfw3.h>

#include <memory>

#include <msg.hpp>

#include "actionTable.hpp"
#include "scene.hpp"

namespace sub3000
{

  class deltaTime_t final
  {
    double start;

    deltaTime_t(const deltaTime_t&) = delete;
    deltaTime_t(deltaTime_t&&) = delete;

    deltaTime_t& operator=(const deltaTime_t&) = delete;
    deltaTime_t& operator=(deltaTime_t&&) = delete;

  public:

    double Mark()
    {
      double now = glfwGetTime();
      double result = now - this->start;
      this->start = now;
      return result;
    }

    deltaTime_t()
    :start(glfwGetTime())
    {
      ;
    }

    ~deltaTime_t()
    {
      ;
    }

  };

  void PostToMain(bb::msg_t&& msg);

  class changeScene_t: public bb::msg::basic_t
  {
    sceneID_t sceneID;
  public:

    sceneID_t SceneID() const
    {
      return this->sceneID;
    }

    changeScene_t(sceneID_t sceneID)
    : sceneID(sceneID)
    {
      ;
    }

    changeScene_t(const changeScene_t&) = default;
    changeScene_t& operator=(const changeScene_t&) = default;
    changeScene_t(changeScene_t&&) = default;
    changeScene_t& operator=(changeScene_t&&) = default;
    ~changeScene_t() override = default;

  };

  class exit_t: public bb::msg::basic_t
  {
  public:
    exit_t() { ; }
    ~exit_t() override = default;
  };

  class action_t: public bb::msg::basic_t
  {
    gameAction_t gameAction;
  public:

    gameAction_t GameAction() const
    {
      return this->gameAction;
    }

    action_t(bb::actorPID_t src, gameAction_t gameAction)
    : bb::msg::basic_t(src),
      gameAction(gameAction)
    {
      ;
    }

    action_t(const action_t&) = default;
    action_t& operator=(const action_t&) = default;
    action_t(action_t&&) = default;
    action_t& operator=(action_t&&) = default;
    ~action_t() override = default;
  };

  bool RequestGenerateMap(uint16_t width, uint16_t height, float radius, int sendResultToID);

} // namespace sub3000

#endif /* __SUB3000_HEADER__ */
