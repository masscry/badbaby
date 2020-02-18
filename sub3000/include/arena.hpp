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

  class arenaScene_t final: public scene_t
  {
    bb::shader_t shader;
    bb::mesh_t radar;
    bb::camera_t camera;

    void OnPrepare() override;
    void OnUpdate(double delta) override;
    void OnRender() override;
    void OnCleanup() override;

  public:

    arenaScene_t();
    ~arenaScene_t() override;

  };

} // namespace sub3000





#endif /* __SUB3000_ARENA_HEADER__ */
