/**
 * @file context.hpp
 * 
 * OpenGL context initialization
 * 
 */

#pragma once
#ifndef __BB_RENDER_CONTEXT_HEADER__
#define __BB_RENDER_CONTEXT_HEADER__

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <framebuffer.hpp>

namespace bb
{

  class context_t final
  {
    GLFWwindow* wnd;
    int width;
    int height;
    framebuffer_t canvas;

    context_t();
    ~context_t();

    context_t(const context_t&) = delete;
    context_t(context_t&&) = delete;

    context_t& operator=(const context_t&) = delete;
    context_t& operator=(context_t&&) = delete;

  public:

    framebuffer_t& Canvas()
    {
      return this->canvas;
    }

    int Width() const 
    {
      return this->width;
    }

    int Height() const 
    {
      return this->height;
    }

    static context_t& Instance();

    bool Update();

  };


} // namespace bb

#endif /* __BB_RENDER_CONTEXT_HEADER__ */