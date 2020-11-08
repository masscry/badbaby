/**
 * @file ubo.hpp
 * 
 * Uniform Block Object
 * 
 */

#pragma once
#ifndef __BB_CORE_RENDER_UBO_HEADER__
#define __BB_CORE_RENDER_UBO_HEADER__

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

    bool IsValid() const
    {
      return (this->self != 0);
    }

    uniformBlock_t(uniformBlock_t&& mv);
    uniformBlock_t& operator=(uniformBlock_t&& mv);

    uniformBlock_t();
    ~uniformBlock_t();

    void UpdateData(const void* data, int offset, size_t size);

    static uniformBlock_t CreateUniformBlock(size_t size);
  };

} // namespace bb

#endif /* __CORE_RENDER_CAMERA_HEADER__ */