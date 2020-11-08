/**
 * @file camera.hpp
 * 
 * Simple camera class
 * 
 */

#pragma once
#ifndef __BB_CORE_RENDER_CAMERA_HEADER__
#define __BB_CORE_RENDER_CAMERA_HEADER__

#include <algebra.hpp>
#include <ubo.hpp>

namespace bb
{
  class camera_t final
  {
    struct data_t {
      mat4_t proj;
      mat4_t view;
    } data;

    mutable uniformBlock_t ubo;

    camera_t(const mat4_t& proj, const mat4_t& view);

  public:

    const uniformBlock_t& UniformBlock() const
    {
      if (!this->ubo.IsValid())
      {
        this->ubo = uniformBlock_t::CreateUniformBlock(sizeof(camera_t::data_t));
        this->Update();
      }
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

    void Update() const;

    camera_t(camera_t&&) noexcept;
    camera_t& operator =(camera_t&&) noexcept;

    camera_t(const camera_t&);
    camera_t& operator =(const camera_t&);

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

    node_t(const node_t&) = default;
    node_t& operator = (const node_t&) = default;

    node_t()
    : model(1.0f)
    {
      ;
    }
    ~node_t() = default;

  };

} // namespace bb



#endif /* __BB_CORE_RENDER_CAMERA_HEADER__ */
