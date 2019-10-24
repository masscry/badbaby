/**
 * @file camera.hpp
 * 
 * Simple camera class.
 * 
 */

#pragma once
#ifndef __CORE_RENDER_UBO_HEADER__
#define __CORE_RENDER_UBO_HEADER__

#include <cstddef>

#include <glad/glad.h>

namespace bb
{

  class uniformBlock_t
  {
    friend class shader_t;

    GLuint self;

    uniformBlock_t(const uniformBlock_t&) = delete;
    uniformBlock_t& operator=(const uniformBlock_t&) = delete;

    uniformBlock_t(GLuint self);

  public:

    uniformBlock_t(uniformBlock_t&&);
    uniformBlock_t& operator=(uniformBlock_t&&);
    ~uniformBlock_t();

    static uniformBlock_t CreateUniformBlock(size_t size);
  };

} // namespace bb

#endif /* __CORE_RENDER_CAMERA_HEADER__ */