/**
 * @file arena.hpp
 * 
 * Tactical battle arena
 * 
 */

#pragma once
#ifndef __SUB3000_ARENA_HEADER__
#define __SUB3000_ARENA_HEADER__

#include <scene.hpp>
#include <vao.hpp>
#include <framebuffer.hpp>
#include <sub3000.hpp>
#include <shapes.hpp>
#include <blur.hpp>
#include <camera.hpp>

namespace sub3000
{

  class radarScreen_t final: public scene_t
  {
    bb::framebuffer_t fb;
    bb::shader_t shader;
    bb::mesh_t radar;
    bb::camera_t camera;

    radarScreen_t(const radarScreen_t&) = delete;
    radarScreen_t& operator=(const radarScreen_t&) = delete;

    void OnPrepare() override;
    void OnUpdate(double delta) override;
    void OnRender() override;
    void OnCleanup() override;

  public:

    bb::framebuffer_t& Framebuffer();
    const bb::framebuffer_t& Framebuffer() const;

    radarScreen_t(radarScreen_t&&) = default;
    radarScreen_t& operator=(radarScreen_t&&) = default;
    radarScreen_t();
    ~radarScreen_t() override = default;

    static radarScreen_t Create();

  };

  inline bb::framebuffer_t& radarScreen_t::Framebuffer()
  {
    return this->fb;
  }

  inline const bb::framebuffer_t& radarScreen_t::Framebuffer() const
  {
    return this->fb;
  }

  class arenaScene_t final: public scene_t
  {
    radarScreen_t radarScreen;

    bb::shader_t shader;
    bb::mesh_t radarPlane;
    bb::camera_t camera;

    void OnPrepare() override;
    void OnUpdate(double delta) override;
    void OnRender() override;
    void OnCleanup() override;

  public:

    arenaScene_t();
    ~arenaScene_t() override = default;

  };

} // namespace sub3000





#endif /* __SUB3000_ARENA_HEADER__ */
