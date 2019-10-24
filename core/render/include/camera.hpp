/**
 * @file camera.hpp
 * 
 * Simple camera class
 * 
 */

#pragma once
#ifndef __BB_CORE_RENDER_CAMERA_HEADER__
#define __BB_CORE_RENDER_CAMERA_HEADER__

#include <glm/mat4x4.hpp>
#include <ubo.hpp>

namespace bb
{

  using mat4_t = glm::mat4;

  class camera_t final
  {
    struct data_t {
      mat4_t proj;
      mat4_t view;
    } data;

    uniformBlock_t ubo;
    
    camera_t(const camera_t&) = delete;
    camera_t& operator =(const camera_t&) = delete;

    camera_t(mat4_t&& proj, mat4_t&& view, uniformBlock_t&& ubo);

  public:

    const uniformBlock_t& UniformBlock() const 
    {
      return this->ubo;
    }

    const mat4_t& Projection() const
    {
      return this->data.proj;
    }

    const mat4_t& View() const
    {
      return this->data.view;
    }

    mat4_t& Projection() 
    {
      return this->data.proj;
    }

    mat4_t& View()
    {
      return this->data.view;
    }

    void Update();

    camera_t(camera_t&&);
    camera_t& operator =(camera_t&&);

    ~camera_t();

    static camera_t Perspective(float fov, float aspect, float nearZ, float farZ);

  };

  
} // namespace bb



#endif /* __BB_CORE_RENDER_CAMERA_HEADER__ */