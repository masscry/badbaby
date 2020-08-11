#include <starrg.hpp>

#include <camera.hpp>
#include <common.hpp>
#include <font.hpp>
#include <image.hpp>
#include <mailbox.hpp>
#include <shader.hpp>
#include <shapes.hpp>
#include <text.hpp>
#include <worker.hpp>

void world_t::UpdateMapUnits()
{
  this->unitsOnMap.clear();
  for (auto it = this->units.begin(), e = this->units.end(); it != e; ++it)
  {
    this->unitsOnMap.insert(
      std::make_pair(
        it->pos,
        it));
  }
}

void world_t::GenerateMap()
{
  auto &engine = RandomEngine();

  this->tiles.clear();
  this->tiles.resize(
    this->mapSize.x * this->mapSize.y,
    cell_t{T_EMPTY, false, false});

  std::discrete_distribution<int> dist{
    15, // T_EMPTY
    5,  // T_GRASS
    3,  // T_ROCK
    5,  // T_GRASS_FLIP
    3,  // T_ROCK_FLIP
    1,  // T_TREE_0
    1,  // T_TREE_1
    1   // T_TREE_2
  };

  std::uniform_int_distribution<int> treeDist(T_TREE_0, T_TREE_2);

  for (auto x = 0; x < this->mapSize.x; ++x)
  {
    this->tiles[x].tile = treeDist(engine);
    this->tiles[this->mapSize.x * (this->mapSize.y - 1) + x].tile = treeDist(engine);
  }

  for (auto y = 1; y < this->mapSize.y - 1; ++y)
  {
    this->tiles[y * this->mapSize.x].tile = treeDist(engine);
    this->tiles[y * this->mapSize.x + this->mapSize.x - 1].tile = treeDist(engine);
  }

  for (auto &item : this->tiles)
  {
    switch (item.tile)
    {
      case T_EMPTY:
        item.tile = static_cast<tile_t>(dist(engine));
        break;
      default:
        break;
    }
  }

  std::uniform_int_distribution<int> posXDist(1, this->mapSize.x - 2);
  std::uniform_int_distribution<int> posYDist(1, this->mapSize.y - 2);

  this->tiles[posYDist(engine) * this->mapSize.x + posXDist(engine)].tile = T_LADDER;

  auto plPos = this->mapSize / 2;
  std::discrete_distribution<int> jumpDist{
    1, 3, 10, 10, 3, 1};

  while (this->CanWalk(plPos) == false)
  {
    plPos.x += jumpDist(engine) - 3;
    plPos.y += jumpDist(engine) - 3;
  }

  this->units.clear();

  this->units.emplace_back(
    unit_t{
      plPos,
      {26, 0},
      80, // int skill; // probability to hit
      60, // int stren; // hit strenght
      60, // int tough; // unit toughness
      95  // int armor; // armor rating
    });

  for (int i = 0; i < 10; ++i)
  {
    auto pos = glm::ivec2{
      posXDist(engine),
      posYDist(engine)};

    while (this->CanWalk(pos) == false)
    {
      pos.x += jumpDist(engine) - 3;
      pos.y += jumpDist(engine) - 3;
    }

    this->units.emplace_back(
      unit_t{
        pos,
        {26, 2},
        20, // int skill; // probability to hit
        10, // int stren; // hit strenght
        10, // int tough; // unit toughness
        5   // int armor; // armor rating
      });
    this->UpdateMapUnits();
  }

  this->timePassed = 0.0;
}

void world_t::CastLight(
  glm::ivec2 pos,
  int radius,
  int row,
  float startSlope,
  float endSlope,
  int xx,
  int xy,
  int yx,
  int yy)
{
  if (startSlope < endSlope)
  {
    return;
  }
  auto nextStartSlope = startSlope;
  for (auto i = row; i <= radius; i++)
  {
    bool blocked = false;
    for (auto dx = -i, dy = -i; dx <= 0; dx++)
    {
      auto l_slope = (dx - 0.5f) / (dy + 0.5f);
      auto r_slope = (dx + 0.5f) / (dy - 0.5f);
      if (startSlope < r_slope)
      {
        continue;
      }
      else if (endSlope > l_slope)
      {
        break;
      }

      auto sax = dx * xx + dy * xy;
      auto say = dx * yx + dy * yy;
      if ((sax < 0 && (uint)std::abs(sax) > pos.x) ||
          (say < 0 && (uint)std::abs(say) > pos.y))
      {
        continue;
      }
      auto ax = pos.x + sax;
      auto ay = pos.y + say;
      if (ax >= this->mapSize.x || ay >= this->mapSize.y)
      {
        continue;
      }

      auto radius2 = radius * radius;
      if ((int)(dx * dx + dy * dy) < radius2)
      {
        this->tiles[ay * this->mapSize.x + ax].visible = true;
      }

      if (blocked)
      {
        if (!this->CanWalk(glm::ivec2{ax, ay}))
        {
          nextStartSlope = r_slope;
          continue;
        }
        else
        {
          blocked = false;
          startSlope = nextStartSlope;
        }
      }
      else if (!this->CanWalk(glm::ivec2{ax, ay}))
      {
        blocked = true;
        nextStartSlope = r_slope;
        this->CastLight(pos, radius, i + 1, startSlope, l_slope, xx, xy, yx, yy);
      }
    }
    if (blocked)
    {
      break;
    }
  }
}

void world_t::UpdateFOV(glm::ivec2 pos, int radius)
{
  static const int multipliers[4][8] = {
    {1, 0, 0, -1, -1, 0, 0, 1},
    {0, 1, -1, 0, 0, -1, 1, 0},
    {0, 1, 1, 0, 0, -1, -1, 0},
    {1, 0, 0, 1, -1, 0, 0, -1}};

  for (int i = 0; i < 8; i++)
  {
    this->CastLight(
      pos,
      radius,
      1,
      1.0,
      0.0,
      multipliers[0][i],
      multipliers[1][i],
      multipliers[2][i],
      multipliers[3][i]);
  }
}

bb::meshDesc_t world_t::BuildTileMap()
{
  std::vector<glm::vec2> posVec;
  std::vector<glm::vec2> uvVec;
  std::vector<glm::vec3> colVec;
  std::vector<uint16_t> indVec;
  bb::meshDesc_t result;

  glm::ivec2 playerPos;

  if (!this->units.empty())
  {
    playerPos = this->units[0].pos;
  }

  posVec.reserve(mapSize.y * mapSize.x * 4);
  uvVec.reserve(mapSize.y * mapSize.x * 4);
  colVec.reserve(mapSize.y * mapSize.x * 4);
  indVec.reserve(mapSize.y * mapSize.x * 6);

  int indOffset = 0;

  for (auto &cell : this->tiles)
  {
    cell.shadow = cell.visible || cell.shadow;
    cell.visible = false;
  }

  this->tiles[this->units[0].pos.y * mapSize.x + this->units[0].pos.x].visible = true;

  this->UpdateFOV(playerPos, 10);
  for (auto &cell : this->tiles)
  {
    cell.shadow = cell.shadow && (!cell.visible);
  }

  for (auto y = 0; y < mapSize.y; ++y)
  {
    for (auto x = 0; x < mapSize.x; ++x)
    {
      auto &cell = this->tiles[y * mapSize.x + x];
      if ((cell.visible == false) && (cell.shadow == false))
      {
        continue;
      }

      auto color = cell.shadow ? glm::vec3(0.5f) : glm::vec3(1.0f);
      auto tile = tileID[cell.tile];
      auto unit = unitsOnMap.find(glm::ivec2(x, y));

      if ((unit != unitsOnMap.end()) && (cell.shadow == false))
      {
        tile.pos = unit->second->sprite;
        tile.canWalk = false;
        tile.flip = false;
      }

      glm::vec2 offset(
        static_cast<float>(tile.pos.x) / static_cast<float>(tileCount.x),
        static_cast<float>(tile.pos.y) / static_cast<float>(tileCount.y));

      glm::vec2 vPos[4];
      glm::vec2 vUV[4];
      glm::vec3 vCol[4];
      uint16_t vInd[6];

      static_assert(sizeof(vUV) == sizeof(vConstUV), "Sizes must be equal");
      static_assert(sizeof(vUV) == sizeof(vConstFlipUV), "Sizes must be equal");
      static_assert(sizeof(vPos) == sizeof(vConstPos), "Sizes must be equal");
      static_assert(sizeof(vInd) == sizeof(vConstInd), "Sizes must be equal");

      glm::vec2 pos = {x * tileSize.x, y * tileSize.y};

      memcpy(vPos, vConstPos, sizeof(vPos));
      memcpy(vUV, tile.flip ? vConstFlipUV : vConstUV, sizeof(vUV));
      memcpy(vInd, vConstInd, sizeof(vConstInd));

      for (auto& i: vCol)
      {
        i = color;
      }

      for (auto& i: vPos)
      {
        i += pos;
      }

      for (auto& i: vUV)
      {
        i += offset;
      }

      for (auto& i: vInd)
      {
        i = static_cast<uint16_t>((i + indOffset) & 0xFFFF);
      }

      posVec.insert(
        posVec.end(),
        std::begin(vPos),
        std::end(vPos));

      uvVec.insert(
        uvVec.end(),
        std::begin(vUV),
        std::end(vUV));

      indVec.insert(
        indVec.end(),
        std::begin(vInd),
        std::end(vInd));

      colVec.insert(
        colVec.end(),
        std::begin(vCol),
        std::end(vCol));

      indOffset += 4;
    }
  }

  result.Buffers().emplace_back(
    bb::MakeVertexBuffer(std::move(posVec)));
  result.Buffers().emplace_back(
    bb::MakeVertexBuffer(std::move(uvVec)));
  result.Buffers().emplace_back(
    bb::MakeVertexBuffer(std::move(colVec)));
  result.Indecies() = bb::MakeIndexBuffer(std::move(indVec));
  result.SetDrawMode(GL_TRIANGLES);
  return result;
}

bb::msg::result_t world_t::OnProcessMessage(const bb::actor_t &, const bb::msg::basic_t &msg)
{
  if (auto key = bb::msg::As<bb::msg::keyEvent_t>(msg))
  {
    switch (key->Press())
    {
      case GLFW_RELEASE:
      {
        if (key->Key() == GLFW_KEY_ESCAPE)
        {
          bb::postOffice_t::Instance().Post(
            "starrg",
            bb::IssuePoison());
        }
      }
      break;
      case GLFW_PRESS:
      case GLFW_REPEAT:
      {
        switch (key->Key())
        {
          case GLFW_KEY_RIGHT:
            bb::postOffice_t::Instance().Post(
              "StarWorld",
              bb::Issue<bb::msg::dataMsg_t<action_t>>(
                action_t{GLFW_KEY_KP_6},
                -1));
            break;
          case GLFW_KEY_LEFT:
            bb::postOffice_t::Instance().Post(
              "StarWorld",
              bb::Issue<bb::msg::dataMsg_t<action_t>>(
                action_t{GLFW_KEY_KP_4},
                -1));
            break;
          case GLFW_KEY_DOWN:
            bb::postOffice_t::Instance().Post(
              "StarWorld",
              bb::Issue<bb::msg::dataMsg_t<action_t>>(
                action_t{GLFW_KEY_KP_2},
                -1));
            break;
          case GLFW_KEY_UP:
            bb::postOffice_t::Instance().Post(
              "StarWorld",
              bb::Issue<bb::msg::dataMsg_t<action_t>>(
                action_t{GLFW_KEY_KP_8},
                -1));
            break;
          case GLFW_KEY_KP_1:
          case GLFW_KEY_KP_2:
          case GLFW_KEY_KP_3:
          case GLFW_KEY_KP_4:
          case GLFW_KEY_KP_6:
          case GLFW_KEY_KP_7:
          case GLFW_KEY_KP_8:
          case GLFW_KEY_KP_9:
            bb::postOffice_t::Instance().Post(
              "StarWorld",
              bb::Issue<bb::msg::dataMsg_t<action_t>>(
                action_t{key->Key()},
                -1));
            break;
          default:
            break;
        }
      }
      break;
      default:
        assert(0);
        break;
    }
    return bb::msg::result_t::complete;
  }

  const double sq2 = 1.41421356237;

  if (auto action = bb::msg::As<bb::msg::dataMsg_t<action_t>>(msg))
  {
    if (!this->units.empty())
    {
      auto &player = this->units[0];

      glm::ivec2 newPos = player.pos;
      switch (action->Data().key)
      {
        case GLFW_KEY_KP_1:
          --newPos.x;
          ++newPos.y;
          this->timePassed += sq2;
          break;
        case GLFW_KEY_KP_2:
          ++newPos.y;
          this->timePassed += 1.0;
          break;
        case GLFW_KEY_KP_3:
          ++newPos.x;
          ++newPos.y;
          this->timePassed += sq2;
          break;
        case GLFW_KEY_KP_4:
          --newPos.x;
          this->timePassed += 1.0;
          break;
        case GLFW_KEY_KP_6:
          ++newPos.x;
          this->timePassed += 1.0;
          break;
        case GLFW_KEY_KP_7:
          --newPos.x;
          --newPos.y;
          this->timePassed += sq2;
          break;
        case GLFW_KEY_KP_8:
          --newPos.y;
          this->timePassed += 1.0;
          break;
        case GLFW_KEY_KP_9:
          ++newPos.x;
          --newPos.y;
          this->timePassed += sq2;
          break;
      }

      auto target = this->unitsOnMap.find(newPos);
      if (target != this->unitsOnMap.end())
      {
        auto &engine = RandomEngine();
        std::uniform_int_distribution<int> dice(0, 100);

        auto playerStats = this->units[0];
        auto targetStats = *target->second;

        int hitRoll = dice(engine);
        if (hitRoll <= playerStats.skill)
        { // has hit
          int woundRoll = dice(engine);
          if (woundRoll <= (playerStats.stren - targetStats.tough))
          { // has wounded
            int armorSave = dice(engine);
            if (armorSave > targetStats.armor)
            { // armor failed
              this->units.erase(target->second);
              this->UpdateMapUnits();
            }
          }
        }
      }

      if (this->CanWalk(newPos))
      {
        player.pos = newPos;

        auto tileInfo = this->TileInfo(player.pos);
        if (tileInfo.ladder)
        {
          this->GenerateMap();
        }
        else
        {
          auto &engine = RandomEngine();
          std::discrete_distribution<int> deltaMove = {
            1, 3, 1};

          while (this->timePassed > 3.0)
          {
            for (auto unit = this->units.begin() + 1, e = this->units.end(); unit != e; ++unit)
            {
              newPos.x = unit->pos.x + deltaMove(engine) - 1;
              newPos.y = unit->pos.y + deltaMove(engine) - 1;
              if (this->CanWalk(newPos))
              {
                unit->pos = newPos;
                this->UpdateMapUnits();
              }
            }
            this->timePassed -= 3.0;
          }
          this->UpdateMapUnits();
        }

        bb::postOffice_t::Instance().Post(
          "StarView",
          bb::Issue<bb::msg::dataMsg_t<bb::meshDesc_t>>(
            this->BuildTileMap(),
            -1));
      }
    }
    return bb::msg::result_t::complete;
  }

  // unknown message
  assert(0);
  return bb::msg::result_t::error;
}

const char* world_t::DefaultName() const
{
  return "StarWorld";
}

tileInfo_t world_t::TileInfo(glm::ivec2 pos) const
{
  if ((pos.x < this->mapSize.x) && (pos.y < this->mapSize.y) && (pos.x >= 0) && (pos.y >= 0))
  {
    return tileID[this->tiles[pos.y * mapSize.x + pos.x].tile];
  }
  return tileID[T_TREE_0];
}

bool world_t::CanWalk(glm::ivec2 pos) const
{
  if ((pos.x < this->mapSize.x) && (pos.y < this->mapSize.y) && (pos.x >= 0) && (pos.y >= 0))
  {
    auto tile = tileID[this->tiles[pos.y * mapSize.x + pos.x].tile];
    if (this->unitsOnMap.find(pos) == this->unitsOnMap.end())
    {
      return tile.canWalk;
    }
  }
  return false;
}

world_t::world_t(glm::ivec2 mapSize)
: mapSize(mapSize)
{
  this->GenerateMap();

  bb::postOffice_t::Instance().Post(
    "StarView",
    bb::Issue<bb::msg::dataMsg_t<bb::meshDesc_t>>(
      this->BuildTileMap(),
      -1
    )
  );
}

world_t::~world_t()
{
}
