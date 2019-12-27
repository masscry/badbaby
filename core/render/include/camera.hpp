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
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ubo.hpp>

namespace bb
{

  using mat4_t = glm::mat4;

  using vec2_t = glm::vec2;
  using vec3_t = glm::vec3;
  using vec4_t = glm::vec4;

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

    camera_t();
    ~camera_t();

    static camera_t Perspective(float fov, float aspect, float nearZ, float farZ);
    static camera_t Orthogonal(float left, float right, float bottom, float top);

  };

  class node_t final
  {
    mat4_t model;

  public:

    const mat4_t& Model() const
    {
      return this->model;
    }

    void Reset()
    {
      this->model = mat4_t(1.0f);
    }

    void Scale(vec3_t v)
    {
      this->model = glm::scale(this->model, v);
    }

    void Translate(vec3_t v)
    {
      this->model = glm::translate(this->model, v);
    }

    void Rotate(vec3_t v, float a)
    {
      this->model = glm::rotate(this->model, a, v);
    }

    node_t(node_t&&) = default;
    node_t& operator =(node_t&&) = default;

    node_t()
    : model(1.0f)
    {
      ;
    }
    ~node_t() = default;

  };

  
} // namespace bb



#endif /* __BB_CORE_RENDER_CAMERA_HEADER__ */