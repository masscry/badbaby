#pragma once
#ifndef TACWAR_GAME_HEADER
#define TACWAR_GAME_HEADER

#include <tacwar.hpp>

#include <deque>

namespace tac
{

  struct trooper_t
  {
    glm::vec2 pos;
    float angle;
  };

  enum class gameMode_t
  {
    select,
    move
  };

  class game_t final: public scene_t 
  {
    bb::camera_t camera;
    bb::shader_t spriteShader;
    bb::mesh_t sprite;
    bb::texture_t spriteTex;

    using troop_t = std::deque<trooper_t>;

    troop_t troop;
    troop_t::iterator sel;
    gameMode_t mode;

    float unitDir;
    glm::vec2 oldPos;
    glm::vec2 newPos;
    double time;
    double fulltime;
    double timeMult;

    void OnClick() override;
    void OnUpdate(double dt) override;
    void OnRender() override;
    void OnAction(int action) override;

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