/**
 * @file camera.hpp
 * 
 * Simple camera class.
 * 
 */

#pragma once
#ifndef __CORE_RENDER_CAMERA_HEADER__
#define __CORE_RENDER_CAMERA_HEADER__

#include <glm/mat4x4.hpp>
#include <glad/glad.h>

namespace bb
{

  class uniformBlock_t
  {
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

  using mat4_t = glm::mat4;

  class camera_t final
  {
    mat4_t view;
    mat4_t proj;

    uniformBlock_t block;

  public:



  };

} // namespace bb

#endif /* __CORE_RENDER_CAMERA_HEADER__ */