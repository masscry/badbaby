/**
 * @file arena.hpp
 * 
 * Tactical battle arena
 * 
 */

#pragma once
#ifndef __SUB3000_ARENA_HEADER__
#define __SUB3000_ARENA_HEADER__

#include <screen_t.hpp>
#include <status_t.hpp>

namespace sub3000
{

  class arenaScene_t final: public scene_t
  {
    radar::screen_t radarScreen;
    bb::mesh_t radarPlane;

    radar::status_t radarStatus;
    bb::mesh_t radarStatusPlane;

    bb::shader_t shader;
    bb::camera_t camera;

    bb::mailbox_t::shared_t box;

    int configWatch;

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
