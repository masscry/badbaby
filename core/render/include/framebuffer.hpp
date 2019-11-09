/**
 * @file framebuffer.hpp
 * 
 * Basic framebuffer routines.
 * 
 */

#pragma once
#ifndef __BB_CORE_RENDER_FRAMEBUFFER_HEADER__
#define __BB_CORE_RENDER_FRAMEBUFFER_HEADER__

#include <glad/glad.h>

#include <texture.hpp>

namespace bb
{

  class framebuffer_t final
  {
    texture_t tex;
    GLuint self;

    framebuffer_t(const framebuffer_t&) = delete;
    framebuffer_t& operator=(const framebuffer_t&) = delete;

  public:

    const texture_t& Texture()
    {
      return this->tex;
    }

    framebuffer_t(framebuffer_t&&);
    framebuffer_t& operator=(framebuffer_t&&);

    framebuffer_t();
    framebuffer_t(int width, int height);
    ~framebuffer_t();

    static void Bind(const framebuffer_t& fb);
    static void RenderToScreen();

  };

} // namespace bb

#endif /* __BB_CORE_RENDER_FRAMEBUFFER_HEADER__ */
