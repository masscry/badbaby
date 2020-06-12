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
#include <sound.hpp>

namespace sub3000
{

  enum class sounds_t: uint32_t
  {
    button,
    engine_on,
    engine_off
  };

  class arenaScene_t final: public scene_t
  {
    radar::screen_t radarScreen;
    bb::mesh_t radarPlane;

    radar::status_t radarStatus;
    bb::mesh_t radarStatusPlane;

    bb::shader_t shader;
    bb::camera_t camera;

    bb::mailbox_t::shared_t box;

    bb::sound_t::stream_t music;
    bb::sound_t::sample_t humming;
    bb::sound_t::sample_t engine;
    bb::sound_t::sample_t button;

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
