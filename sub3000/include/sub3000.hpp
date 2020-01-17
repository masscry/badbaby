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

  void PostToMain(bb::msg_t msg);

  enum mainMessage_t
  {
    nop = 0,
    change_scene,
    exit,
    action
  };

  bool RequestGenerateMap(uint16_t width, uint16_t height, float radius, int sendResultToID);

} // namespace sub3000

#include "actionTable.hpp"

#endif /* __SUB3000_HEADER__ */
