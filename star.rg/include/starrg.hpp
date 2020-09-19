#pragma once
#ifndef STARRG_HEADER
#define STARRG_HEADER

#include <common.hpp>
#include <context.hpp>
#include <mailbox.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <image.hpp>
#include <worker.hpp>
#include <font.hpp>
#include <text.hpp>
#include <role.hpp>

#include <random>

#include <components.hpp>

std::mt19937& RandomEngine();

enum tile_t
{
  T_EMPTY = 0,
  T_GRASS,
  T_ROCK,
  T_GRASS_FLIP,
  T_ROCK_FLIP,
  T_TREE_0,
  T_TREE_1,
  T_TREE_2,
  T_LADDER,
  T_TOTAL
};

struct tileInfo_t
{
  glm::ivec2 pos;
  bool flip;
  bool canWalk;
  bool ladder;
};

const glm::vec2 tileSize  = { 16.0f,  16.0f   };
const glm::vec2 tileCount = { 48, 22 };
const glm::vec2 tileTSize = { 1.0f/tileCount.x, 1.0f/tileCount.y };

const glm::vec3 vConstPos[4] = {
  {  0.0f,       0.0f,       0.0f },
  {  tileSize.x, 0.0f,       0.0f },
  {  tileSize.x, tileSize.y, 0.0f },
  {  0.0f,       tileSize.y, 0.0f }
};

const glm::vec2 vConstUV[4] = {
  { 0.0f,        0.0f },
  { tileTSize.x, 0.0f },
  { tileTSize.x, tileTSize.y },
  { 0.0f,        tileTSize.y }
};

const glm::vec2 vConstFlipUV[4] = {
  { tileTSize.x, 0.0f },
  { 0.0f,        0.0f },
  { 0.0f,        tileTSize.y },
  { tileTSize.x, tileTSize.y }
};

const uint16_t vConstInd[6] = {
  0, 1, 2, 0, 2, 3
};

#include "tileDB.hpp"

using unit_t = sr::entityID_t;

struct ivecKey_t
{
  size_t operator()(const glm::ivec2& k) const
  {
    return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
  }
  
  bool operator()(const glm::ivec2& a, const glm::ivec2& b)const
  {
    return a.x == b.x && a.y == b.y;
  }
};

struct action_t
{
  int key;
};

struct cell_t
{
  int tile;
  bool visible;
  bool shadow;
};

#include "world.hpp"

class starrg_t
{
  bb::context_t& context;
  bb::camera_t camera;
  bb::shader_t mapShader;
  bb::shader_t fontShader;
  bb::texture_t tileset;
  bb::font_t font;
  bb::textDynamic_t text;
  bb::textDynamic_t log;
  bb::mesh_t map;
  bb::mesh_t unit;
  bb::mailbox_t::shared_t box;
  bb::actorPID_t world;
  std::deque<std::string> logLines;
  std::string logText;

  double time;

public:

  starrg_t(const starrg_t&) = delete;
  starrg_t(starrg_t&&) = delete;

  starrg_t& operator=(const starrg_t&) = delete;
  starrg_t& operator=(starrg_t&&) = delete;

  void Update(double dt);
  void Render();

  starrg_t(bb::context_t& context);
  ~starrg_t();

};

#endif /* STARRG_HEADER */