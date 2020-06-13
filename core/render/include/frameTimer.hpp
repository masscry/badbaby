/**
 * @file frameTimer.hpp
 * 
 * Frame time counter class
 * 
 */

#pragma once
#ifndef __BB_RENDER_FRAME_TIMER_HEADER__
#define __BB_RENDER_FRAME_TIMER_HEADER__

#include <context.hpp>

namespace bb
{

  class frameTimer_t final
  {
    double start;

  public:
    frameTimer_t()
    : start(glfwGetTime())
    {
      ;
    }

    frameTimer_t(const frameTimer_t&) = default;
    frameTimer_t(frameTimer_t&&) = default;
    frameTimer_t& operator = (const frameTimer_t&) = default;
    frameTimer_t& operator = (frameTimer_t&&) = default;

    void Reset()
    {
      this->start = glfwGetTime();
    }

    double Delta()
    {
      auto finish = glfwGetTime();
      auto delta = finish - this->start;
      this->start = finish;
      return delta;
    }

    ~frameTimer_t()
    {
      ;
    }

  };

} // namespace bb

#endif /* __BB_RENDER_FRAME_TIMER_HEADER__ */