/**
 * @file framebuffer.hpp
 * 
 * Basic framebuffer routines.
 * 
 */

#pragma once
#ifndef __BB_RENDER_FRAMEBUFFER_HEADER__
#define __BB_RENDER_FRAMEBUFFER_HEADER__

#include <glad/glad.h>

namespace bb
{

  class framebuffer_t final
  {
    GLuint self;
    GLuint tex;
    GLuint rbo;

    framebuffer_t(const framebuffer_t&) = delete;
    framebuffer_t& operator=(const framebuffer_t&) = delete;

  public:

    framebuffer_t(framebuffer_t&&);
    framebuffer_t& operator=(framebuffer_t&&);

    framebuffer_t();
    framebuffer_t(int width, int height);
    ~framebuffer_t();
  };

} // namespace bb

#endif /* __BB_RENDER_FRAMEBUFFER_HEADER__ */
