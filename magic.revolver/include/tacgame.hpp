#pragma once
#ifndef TACWAR_GAME_HEADER
#define TACWAR_GAME_HEADER

#include <tacwar.hpp>

#include <deque>

#include <common.hpp>

namespace tac
{

  class game_t final: public scene_t 
  {
    bb::camera_t camera;
    bb::shader_t spriteShader;
    bb::mesh_t sprite;
    bb::texture_t spriteTex;

    void OnClick() override;
    void OnUpdate(double dt) override;
    void OnRender() override;
    void OnAction(int action) override;
    void OnMouse(int btn, int press) override;

    void Prepare();
    void Cleanup();

  public:

    game_t()
    : scene_t(tac::state_t::game)
    {
      this->Prepare();
    }

    ~game_t()
    {
      this->Cleanup();
    }

  };

}

#endif /* TACWAR_GAME_HEADER */