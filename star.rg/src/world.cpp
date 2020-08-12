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

#include <sstream>

void world_t::UpdateMapUnits()
{
  this->unitsOnMap.clear();
  for (auto it = this->units.begin(), e = this->units.end(); it != e; ++it)
  {
    this->unitsOnMap.insert(
      std::make_pair(
        it->pos,
        it)
    );
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
    this->Tiles(glm::ivec2{x, 0}).tile = treeDist(engine);
    this->Tiles(glm::ivec2{x, this->mapSize.y-1}).tile = treeDist(engine);
  }

  for (auto y = 1; y < this->mapSize.y - 1; ++y)
  {
    this->Tiles(glm::ivec2{0, y}).tile = treeDist(engine);
    this->Tiles(glm::ivec2{this->mapSize.x-1, y}).tile = treeDist(engine);
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

  this->Tiles(glm::ivec2{posXDist(engine), posYDist(engine)}).tile = T_LADDER;

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
      95, // int armor; // armor rating
      US_NONE, // int status; // unit status
      {0.0f, 0.0f}  // int side; // unit side 
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
        5,  // int armor; // armor rating
        US_NONE, // int status; // unit status
        {0.0f, 0.0f}  // int side; // unit side 
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
        auto& cell = this->Tiles(glm::ivec2{ax, ay});
        if ((cell.tile == T_LADDER) && (cell.visible == false) && (cell.shadow == false))
        {
          bb::postOffice_t::Instance().Post(
            "StarView",
            bb::Issue<bb::msg::dataMsg_t<std::string>>(
              "Конан нашел лестницу!",
              -1
            )
          );
        }
        this->Tiles(glm::ivec2{ax, ay}).visible = true;
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
      1.0f,
      0.0f,
      multipliers[0][i],
      multipliers[1][i],
      multipliers[2][i],
      multipliers[3][i]);
  }
}

struct quadData_t
{
  glm::vec3 vPos[4];
  glm::vec2 vUV[4];
  glm::vec3 vCol[4];
  glm::vec2 vShim[4];
  uint16_t vInd[6];
};

struct triData_t
{
  std::vector<glm::vec3> pos;
  std::vector<glm::vec2> uv;
  std::vector<glm::vec3> col;
  std::vector<glm::vec2> shim;
  std::vector<uint16_t> ind;

  bb::meshDesc_t Create()
  {
    bb::meshDesc_t result;
    result.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(this->pos)));
    result.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(this->uv)));
    result.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(this->col)));
    result.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(this->shim)));
    result.Indecies() = bb::MakeIndexBuffer(std::move(this->ind));
    result.SetDrawMode(GL_TRIANGLES);
    return result;
  }

  void Add(const quadData_t& data)
  {
    this->pos.insert(
      this->pos.end(),
      std::begin(data.vPos),
      std::end(data.vPos));

    this->uv.insert(
      this->uv.end(),
      std::begin(data.vUV),
      std::end(data.vUV));

    this->col.insert(
      this->col.end(),
      std::begin(data.vCol),
      std::end(data.vCol));

    this->shim.insert(
      this->shim.end(),
      std::begin(data.vShim),
      std::end(data.vShim)
    );

    this->ind.insert(
      this->ind.end(),
      std::begin(data.vInd),
      std::end(data.vInd)
    );
  }

};

quadData_t CreateQuad(glm::ivec2 sprite, glm::vec2 pos, glm::vec3 col, uint16_t indOffset, bool flip)
{
  quadData_t result;

  glm::vec2 uvOffset(
    static_cast<float>(sprite.x) / static_cast<float>(tileCount.x),
    static_cast<float>(sprite.y) / static_cast<float>(tileCount.y)
  );

  static_assert(sizeof(result.vUV) == sizeof(vConstUV), "Sizes must be equal");
  static_assert(sizeof(result.vUV) == sizeof(vConstFlipUV), "Sizes must be equal");
  static_assert(sizeof(result.vPos) == sizeof(vConstPos), "Sizes must be equal");
  static_assert(sizeof(result.vInd) == sizeof(vConstInd), "Sizes must be equal");

  memcpy(result.vPos, vConstPos, sizeof(result.vPos));
  memcpy(result.vUV, flip ? vConstFlipUV : vConstUV, sizeof(result.vUV));
  memcpy(result.vInd, vConstInd, sizeof(result.vInd));

  for (auto &i : result.vPos)
  {
    i.x += pos.x;
    i.y += pos.y;
  }

  for (auto &i : result.vCol)
  {
    i = col;
  }

  for (auto &i : result.vUV)
  {
    i += uvOffset;
  }

  for (auto& v: result.vShim)
  {
    v = glm::vec2(0.0f);
  }

  for (auto &i : result.vInd)
  {
    i = static_cast<uint16_t>((i + indOffset) & 0xFFFF);
  }

  return result;
}


bb::meshDesc_t world_t::BuildUnits()
{
  triData_t vec;
  bb::meshDesc_t result;

  if (this->units.empty())
  {
    return bb::meshDesc_t();
  }

  vec.pos.reserve(this->units.size() * 4);
  vec.uv.reserve(this->units.size() * 4);
  vec.col.reserve(this->units.size() * 4);
  vec.shim.reserve(this->units.size() * 4);
  vec.ind.reserve(this->units.size() * 6);

  int indOffset = 0;

  for (auto& unit : this->units)
  {
    auto info = this->Tiles(unit.pos);

    if (!info.visible)
    {
      continue;
    }

    glm::vec2 pos = {unit.pos.x * tileSize.x, unit.pos.y * tileSize.y};

    quadData_t q = CreateQuad(unit.sprite, pos, glm::vec3(1.0f), indOffset, false);
    for (auto& v: q.vPos)
    {
      v.z = 1.0f;
    }

    switch (unit.status)
    {
    case US_NONE:
      break;
    case US_MISS:
      for (auto& v: q.vShim)
      {
        v = unit.side*10.0f;
      }
      unit.status = US_NONE;
      break;
    case US_ARMOR:
      {
        quadData_t shield = CreateQuad(
          glm::ivec2{39, 3},
          pos,
          glm::vec3(1.0f),
          indOffset+4,
          false
        );
        for (auto& v: q.vCol)
        {
          v *= 0.3f;
        }
        for (auto& v: shield.vPos)
        {
          v.z = 2.0f;
        }
        for (auto& v: shield.vShim)
        {
          v = unit.side*14.0f;
        }
        vec.Add(shield);
        indOffset += 4;
        unit.status = US_NONE;
      }
      break;
    case US_SAVE:
      for (auto& v: q.vShim)
      {
        v = glm::vec2(unit.side.y, -unit.side.x)*10.0f;
      }
      unit.status = US_NONE;
      break;
    case US_DEAD:
      {
        for (auto& v: q.vCol)
        {
          v = glm::vec3(0.8f,0.3f,0.3f);
        }
        auto temp = q.vUV[3];
        q.vUV[3] = q.vUV[2];
        q.vUV[2] = q.vUV[1];
        q.vUV[1] = q.vUV[0];
        q.vUV[0] = temp;
        for (auto& v: q.vPos)
        {
          v.z = 0.1f;
        }
      }
      break;
    default:
      assert(0);
      break;
    }

    vec.Add(q);
    indOffset += 4;
  }

  return vec.Create();
}

bb::meshDesc_t world_t::BuildTileMap()
{
  triData_t vec;
  bb::meshDesc_t result;

  glm::ivec2 playerPos;

  if (!this->units.empty())
  {
    playerPos = this->units[0].pos;
  }

  vec.pos.reserve(mapSize.y * mapSize.x * 4);
  vec.uv.reserve(mapSize.y * mapSize.x * 4);
  vec.col.reserve(mapSize.y * mapSize.x * 4);
  vec.shim.reserve(mapSize.y * mapSize.x * 4);
  vec.ind.reserve(mapSize.y * mapSize.x * 6);

  int indOffset = 0;

  for (auto &cell : this->tiles)
  {
    cell.shadow = cell.visible || cell.shadow;
    cell.visible = false;
  }

  this->Tiles(this->units[0].pos).visible = true;

  this->UpdateFOV(playerPos, 10);
  for (auto &cell : this->tiles)
  {
    cell.shadow = cell.shadow && (!cell.visible);
  }

  for (auto y = 0; y < mapSize.y; ++y)
  {
    for (auto x = 0; x < mapSize.x; ++x)
    {
      auto &cell = this->Tiles(glm::ivec2{x, y});
      if ((cell.visible == false) && (cell.shadow == false))
      {
        continue;
      }

      auto color = cell.shadow ? glm::vec3(0.3f) : glm::vec3(0.6f);
      auto tile = tileID[cell.tile];
      glm::vec2 pos = {x * tileSize.x, y * tileSize.y};

      quadData_t q = CreateQuad(tile.pos, pos, color, indOffset, tile.flip);

      vec.Add(q);
      indOffset += 4;
    }
  }

  return vec.Create();
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
          ++this->step;
          break;
        case GLFW_KEY_KP_2:
          ++newPos.y;
          this->timePassed += 1.0;
          ++this->step;
          break;
        case GLFW_KEY_KP_3:
          ++newPos.x;
          ++newPos.y;
          this->timePassed += sq2;
          ++this->step;
          break;
        case GLFW_KEY_KP_4:
          --newPos.x;
          this->timePassed += 1.0;
          ++this->step;
          break;
        case GLFW_KEY_KP_6:
          ++newPos.x;
          this->timePassed += 1.0;
          ++this->step;
          break;
        case GLFW_KEY_KP_7:
          --newPos.x;
          --newPos.y;
          this->timePassed += sq2;
          ++this->step;
          break;
        case GLFW_KEY_KP_8:
          --newPos.y;
          this->timePassed += 1.0;
          ++this->step;
          break;
        case GLFW_KEY_KP_9:
          ++newPos.x;
          --newPos.y;
          this->timePassed += sq2;
          ++this->step;
          break;
      }

      auto target = this->unitsOnMap.find(newPos);
      if ((target != this->unitsOnMap.end()) && (target->second->status != US_DEAD))
      {
        std::stringstream logst;
        auto &engine = RandomEngine();
        std::uniform_int_distribution<int> dice(0, 100);

        auto& playerStats = this->units[0];
        auto& targetStats = *target->second;
        targetStats.side = newPos - player.pos;

        logst << "[" << this->step << "]" << "Конан бьёт! ";

        int hitRoll = dice(engine);
        if (hitRoll <= playerStats.skill)
        { // has hit
          int woundRoll = dice(engine);
          if (woundRoll <= (playerStats.stren - targetStats.tough))
          { // has wounded
            int armorSave = dice(engine);
            if (armorSave > targetStats.armor)
            { // armor failed
              logst << "Убил!";
              targetStats.status = US_DEAD;
              this->UpdateMapUnits();
            }
            else
            {
              logst << "Не задел! (S" << armorSave << "<" << targetStats.armor << ')';
              playerStats.status = US_SAVE;
              playerStats.side = player.pos-newPos;
              targetStats.status = US_SAVE;
            }
          }
          else
          {
            logst << "Мечь отскочил от брони! (W" << woundRoll << ">" << playerStats.stren - targetStats.tough << ')';
            playerStats.status = US_MISS;
            playerStats.side = player.pos-newPos;
            targetStats.status = US_ARMOR;
          }
        }
        else
        {
          logst << "Промазал! (H" << hitRoll << ">" << playerStats.skill << ')';
          playerStats.status = US_MISS;
          playerStats.side = player.pos-newPos;
          targetStats.status = US_MISS;
        }

        bb::postOffice_t::Instance().Post(
          "StarView",
          bb::Issue<bb::msg::dataMsg_t<std::string>>(
            logst.str(),
            -1
          )
        );

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
              if (unit->status == US_DEAD)
              {
                continue;
              }
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
          bb::Issue<meshData_t>(
            this->BuildTileMap(),
            meshData_t::M_MAP
          )
        );
      }

      bb::postOffice_t::Instance().Post(
        "StarView",
        bb::Issue<meshData_t>(
          this->BuildUnits(),
          meshData_t::M_UNIT
        )
      );

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
    return tileID[this->Tiles(pos).tile];
  }
  return tileID[T_TREE_0];
}

bool world_t::CanWalk(glm::ivec2 pos) const
{
  if ((pos.x < this->mapSize.x) && (pos.y < this->mapSize.y) && (pos.x >= 0) && (pos.y >= 0))
  {
    auto tile = tileID[this->Tiles(pos).tile];
    auto unit = this->unitsOnMap.find(pos) ;
    if ((unit == this->unitsOnMap.end()) || (unit->second->status == US_DEAD))
    {
      return tile.canWalk;
    }
  }
  return false;
}

world_t::world_t(glm::ivec2 mapSize)
: mapSize(mapSize),
  step(1)
{
  this->GenerateMap();

  bb::postOffice_t::Instance().Post(
    "StarView",
    bb::Issue<meshData_t>(
      this->BuildTileMap(),
      meshData_t::M_MAP
    )
  );
  bb::postOffice_t::Instance().Post(
    "StarView",
    bb::Issue<meshData_t>(
      this->BuildUnits(),
      meshData_t::M_UNIT
    )
  );

}

world_t::~world_t()
{
}
