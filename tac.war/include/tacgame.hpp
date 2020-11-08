#pragma once
#ifndef TACWAR_GAME_HEADER
#define TACWAR_GAME_HEADER

#include <tacwar.hpp>

#include <deque>

#include <common.hpp>

namespace tac
{

  enum teamID_t
  {
    TEAM_FIRST = 0,
    TEAM_ALICE = TEAM_FIRST,
    TEAM_BOB,
    TEAM_TOTAL
  };

  const glm::vec3 teamColor[] = {
    glm::vec3{ 1.0f, 0.1f, 0.1f },
    glm::vec3{ 0.1f, 1.0f, 0.1f }
  };

  static_assert(bb::countof(teamColor) == TEAM_TOTAL, "teamColor must be equal to TEAM_TOTAL");

  struct trooper_t
  {
    glm::vec2 pos;
    float angle;
    int team;
    int flags;
    float maxDist;
    bb::mesh_t* lineMesh;
  };

  enum class gameMode_t
  {
    select,
    dir,
    move,
    rot
  };

  struct segment_t
  {
    glm::vec2 start;
    glm::vec2 finish;
  };

  class game_t final: public scene_t 
  {
    bb::camera_t camera;
    bb::shader_t spriteShader;
    bb::mesh_t sprite;
    bb::texture_t spriteTex;
    bb::mesh_t lineMesh;
    bb::mesh_t level;
    std::deque<bb::mesh_t> troopLineMesh;

  public:

    using troop_t = std::deque<trooper_t>;
    using segments_t = std::deque<segment_t>;

  private:

    troop_t troop;
    troop_t::iterator sel;
    gameMode_t mode;

    float finalDir;
    float unitDir;
    glm::vec2 oldPos;
    glm::vec2 newPos;
    double time;
    double fulltime;
    double timeMult;

    int curTeam;
    int round;
    bool showShade;
    bool shadeChanged;

    segments_t segments;

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