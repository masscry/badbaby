#include <common.hpp>
#include <context.hpp>
#include <mailbox.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <image.hpp>
#include <role.hpp>
#include <worker.hpp>

#include <random>

namespace
{
  bb::mailbox_t::shared_t& Mailbox()
  {
    static auto mail = bb::postOffice_t::Instance().New("starrg");
    return mail;
  }

  const glm::vec2  tileSize  = { 16.0f,  16.0f   };
  const glm::ivec2 tileCount = { 14,    10};
  const glm::vec2  tileTSize = { 1.0f/tileCount.x, 1.0f/tileCount.y };

  glm::vec2 vConstPos[4] = {
    {  0.0f,       0.0f       },
    {  tileSize.x, 0.0f       },
    {  tileSize.x, tileSize.y },
    {  0.0f,       tileSize.y }
  };

  glm::vec2 vConstUV[4] = {
    { 0.0f,        0.0f },
    { tileTSize.x, 0.0f },
    { tileTSize.x, tileTSize.y },
    { 0.0f,        tileTSize.y }
  };

  glm::vec2 vConstFlipUV[4] = {
    { tileTSize.x, 0.0f },
    { 0.0f,        0.0f },
    { 0.0f,        tileTSize.y },
    { tileTSize.x, tileTSize.y }
  };

  uint16_t vConstInd[6] = {
    0, 1, 2, 0, 2, 3
  };

  std::mt19937& RandomEngine()
  {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
  }

}

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

const tileInfo_t tileID[] = {
  { { 1, 1 }, false, true , false }, // T_EMPTY
  { { 5, 4 }, false, true , false }, // T_GRASS
  { { 4, 4 }, false, true , false }, // T_ROCK
  { { 5, 4 }, true , true , false }, // T_GRASS_FLIP
  { { 4, 4 }, true , true , false }, // T_ROCK_FLIP
  { { 4, 5 }, false, false, false }, // T_TREE_0
  { { 5, 5 }, false, false, false }, // T_TREE_1
  { { 6, 5 }, false, false, false }, // T_TREE_2
  { { 4, 3 }, false, true , true  }, // T_LADDER
};

static_assert(bb::countof(tileID) == T_TOTAL, "All tiles must have IDs");

struct unit_t
{
  glm::ivec2 pos;
  glm::ivec2 sprite;
};

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

static const int multipliers[4][8] = {
    {1, 0, 0, -1, -1, 0, 0, 1},
    {0, 1, -1, 0, 0, -1, 1, 0},
    {0, 1, 1, 0, 0, -1, -1, 0},
    {1, 0, 0, 1, -1, 0, 0, -1}
};

class world_t: public bb::role_t
{
  glm::ivec2 mapSize;
  std::vector<cell_t> tiles;
  std::deque<unit_t> units;

  void GenerateMap()
  {
    auto& engine = RandomEngine();

    this->tiles.clear();
    this->tiles.resize(
      this->mapSize.x * this->mapSize.y,
      cell_t{ T_EMPTY, false, false }
    );

    std::discrete_distribution<int> dist {
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
      this->tiles[this->mapSize.x*(this->mapSize.y-1) + x].tile = treeDist(engine);
    }

    for (auto y = 1; y < this->mapSize.y-1; ++y)
    {
      this->tiles[y*this->mapSize.x].tile = treeDist(engine);
      this->tiles[y*this->mapSize.x + this->mapSize.x - 1].tile = treeDist(engine);
    }

    for (auto& item: this->tiles)
    {
      switch(item.tile)
      {
        case T_EMPTY:
          item.tile = static_cast<tile_t>(dist(engine));
          break;
        default:
          break;
      }
    }

    std::uniform_int_distribution<int> posXDist(1, this->mapSize.x-2);
    std::uniform_int_distribution<int> posYDist(1, this->mapSize.y-2);

    this->tiles[posYDist(engine)*this->mapSize.x + posXDist(engine)].tile = T_LADDER;

    auto plPos = this->mapSize/2;
    std::discrete_distribution<int> jumpDist {
      1, 3, 10, 10, 3, 1
    };

    while (this->CanWalk(plPos) == false)
    {
      plPos.x += jumpDist(engine)-3;
      plPos.y += jumpDist(engine)-3;
    }

    if (this->units.empty())
    {
      this->units.emplace_back(
        unit_t{
          plPos,
          { 4, 0 }
        }
      );
    }
    else
    {
      this->units[0].pos = plPos;
    }
  }

  void CastLight(
    glm::ivec2 pos,
    int radius,
    int row,
    float startSlope,
    float endSlope,
    int xx,
    int xy,
    int yx,
    int yy
  )
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
        auto l_slope = (dx - 0.5) / (dy + 0.5);
        auto r_slope = (dx + 0.5) / (dy - 0.5);
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
          this->tiles[ay*this->mapSize.x + ax].visible = true;
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

  void UpdateFOV(glm::ivec2 pos, int radius) 
  {
    for (uint i = 0; i < 8; i++) 
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
        multipliers[3][i]
      );
    }
  }

  bb::meshDesc_t BuildTileMap()
  {
    std::unordered_multimap<glm::ivec2, std::deque<unit_t>::iterator, ivecKey_t, ivecKey_t> unitsOnMap;
    std::vector<glm::vec2> posVec;
    std::vector<glm::vec2> uvVec;
    std::vector<glm::vec3> colVec;
    std::vector<uint16_t> indVec;
    bb::meshDesc_t result;

    glm::ivec2 playerPos;

    for (auto it = this->units.begin(), e = this->units.end(); it != e; ++it)
    {
      unitsOnMap.insert(
        std::make_pair(
          it->pos,
          it
        )
      );
    }

    if (!this->units.empty())
    {
      playerPos = this->units[0].pos;
    }

    posVec.reserve(mapSize.y*mapSize.x*4);
    uvVec.reserve(mapSize.y*mapSize.x*4);
    colVec.reserve(mapSize.y*mapSize.x*4);
    indVec.reserve(mapSize.y*mapSize.x*6);

    uint16_t indOffset = 0;

    for (auto y = 0; y < mapSize.y; ++y)
    {
      for (auto x = 0; x < mapSize.x; ++x)
      {
        auto& cell = this->tiles[y*mapSize.x + x];
        cell.shadow = cell.visible || cell.shadow;
        cell.visible = false;
      }
    }

    this->tiles[this->units[0].pos.y*mapSize.x + this->units[0].pos.x].visible = true;
    
    this->UpdateFOV(playerPos, 10);
    for (auto y = 0; y < mapSize.y; ++y)
    {
      for (auto x = 0; x < mapSize.x; ++x)
      {
        auto& cell = this->tiles[y*mapSize.x + x];
        if (cell.visible)
        {
          cell.shadow = false;
        }
      }
    }

    for (auto y = 0; y < mapSize.y; ++y)
    {
      for (auto x = 0; x < mapSize.x; ++x)
      {
        auto& cell = this->tiles[y*mapSize.x + x];
        if ((cell.visible == false) && (cell.shadow == false))
        {
          continue;
        }

        auto color = cell.shadow?glm::vec3(0.5f):glm::vec3(1.0f);
        auto tile = tileID[cell.tile];
        auto unit = unitsOnMap.find(glm::ivec2(x, y));

        if (unit != unitsOnMap.end())
        {
          tile.pos = unit->second->sprite;
          tile.canWalk = false;
          tile.flip = false;
        }

        glm::vec2 offset(
          static_cast<float>(tile.pos.x)/static_cast<float>(tileCount.x),
          static_cast<float>(tile.pos.y)/static_cast<float>(tileCount.y)
        );

        glm::vec2 vPos[4];
        glm::vec2 vUV[4];
        glm::vec3 vCol[4];
        uint16_t vInd[6];

        static_assert(sizeof(vUV) == sizeof(vConstUV), "Sizes must be equal");
        static_assert(sizeof(vUV) == sizeof(vConstFlipUV), "Sizes must be equal");
        static_assert(sizeof(vPos) == sizeof(vConstPos), "Sizes must be equal");
        static_assert(sizeof(vInd) == sizeof(vConstInd), "Sizes must be equal");

        glm::vec2 pos = { x*tileSize.x, y * tileSize.y };

        memcpy(vPos, vConstPos, sizeof(vPos));
        memcpy(vUV, tile.flip?vConstFlipUV:vConstUV, sizeof(vUV));
        memcpy(vInd, vConstInd, sizeof(vConstInd));

        vCol[0] = color;
        vCol[1] = color;
        vCol[2] = color;
        vCol[3] = color;
    
        vPos[0] += pos;
        vPos[1] += pos;
        vPos[2] += pos;
        vPos[3] += pos;
    
        vUV[0] += offset;
        vUV[1] += offset;
        vUV[2] += offset;
        vUV[3] += offset;

        vInd[0] += indOffset;
        vInd[1] += indOffset;
        vInd[2] += indOffset;
        vInd[3] += indOffset;
        vInd[4] += indOffset;
        vInd[5] += indOffset;

        posVec.insert(
          posVec.end(),
          std::begin(vPos),
          std::end(vPos)
        );

        uvVec.insert(
          uvVec.end(),
          std::begin(vUV),
          std::end(vUV)
        );

        indVec.insert(
          indVec.end(),
          std::begin(vInd),
          std::end(vInd)
        );

        colVec.insert(
          colVec.end(),
          std::begin(vCol),
          std::end(vCol)
        );

        indOffset += 4;

      }
    }

    result.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(posVec))
    );
    result.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(uvVec))
    );
    result.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(colVec))
    );
    result.Indecies() = bb::MakeIndexBuffer(std::move(indVec));
    result.SetDrawMode(GL_TRIANGLES);

    return result;
  }

  bb::msg::result_t OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg) override
  {
    if (auto key = bb::msg::As<bb::msg::keyEvent_t>(msg))
    {
      switch(key->Press())
      {
        case GLFW_RELEASE:
          {
            if (key->Key() == GLFW_KEY_ESCAPE)
            {
              bb::postOffice_t::Instance().Post(
                "starrg",
                bb::IssuePoison()
              );
            }
          }
          break;
        case GLFW_PRESS:
        case GLFW_REPEAT:
          {
            switch(key->Key())
            {
              case GLFW_KEY_RIGHT:
                bb::postOffice_t::Instance().Post(
                  "StarWorld",
                  bb::Issue<bb::msg::dataMsg_t<action_t>>(
                    action_t{ GLFW_KEY_KP_6 },
                    -1
                  )
                );
                break;
              case GLFW_KEY_LEFT:
                bb::postOffice_t::Instance().Post(
                  "StarWorld",
                  bb::Issue<bb::msg::dataMsg_t<action_t>>(
                    action_t{ GLFW_KEY_KP_4 },
                    -1
                  )
                );
                break;
              case GLFW_KEY_DOWN:
                bb::postOffice_t::Instance().Post(
                  "StarWorld",
                  bb::Issue<bb::msg::dataMsg_t<action_t>>(
                    action_t{ GLFW_KEY_KP_2 },
                    -1
                  )
                );
                break;
              case GLFW_KEY_UP:
                bb::postOffice_t::Instance().Post(
                  "StarWorld",
                  bb::Issue<bb::msg::dataMsg_t<action_t>>(
                    action_t{ GLFW_KEY_KP_8 },
                    -1
                  )
                );
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
                    action_t{ key->Key() },
                    -1
                  )
                );
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

    if (auto action = bb::msg::As<bb::msg::dataMsg_t<action_t>>(msg))
    {
      if (!this->units.empty())
      {
        auto& player = this->units[0];

        glm::ivec2 newPos = player.pos;
        switch(action->Data().key)
        {
          case GLFW_KEY_KP_1:
            --newPos.x;
            ++newPos.y;
            break;
          case GLFW_KEY_KP_2:
            ++newPos.y;
            break;
          case GLFW_KEY_KP_3:
            ++newPos.x;
            ++newPos.y;
            break;
          case GLFW_KEY_KP_4:
            --newPos.x;
            break;
          case GLFW_KEY_KP_6:
            ++newPos.x;
            break;
          case GLFW_KEY_KP_7:
            --newPos.x;
            --newPos.y;
            break;
          case GLFW_KEY_KP_8:
            --newPos.y;
            break;
          case GLFW_KEY_KP_9:
            ++newPos.x;
            --newPos.y;
            break;
        }

        if (this->CanWalk(newPos))
        {
          player.pos = newPos;

          auto tileInfo = this->TileInfo(player.pos);
          if (tileInfo.ladder)
          {
            this->GenerateMap();
          }

          bb::postOffice_t::Instance().Post(
            "StarView",
            bb::Issue<bb::msg::dataMsg_t<bb::meshDesc_t>>(
              this->BuildTileMap(),
              -1
            )
          );
        }
      }
      return bb::msg::result_t::complete;
    }

    // unknown message
    assert(0);
    return bb::msg::result_t::error;
  }

public:

  const char* DefaultName() const override
  {
    return "StarWorld";
  }

  tileInfo_t TileInfo(glm::ivec2 pos) const
  {
    if ((pos.x < this->mapSize.x) && (pos.y < this->mapSize.y) && (pos.x >= 0) && (pos.y >= 0))
    {
      return tileID[this->tiles[pos.y*mapSize.x + pos.x].tile];
    }
    return tileID[T_TREE_0];
  }

  bool CanWalk(glm::ivec2 pos) const
  {
    if ((pos.x < this->mapSize.x) && (pos.y < this->mapSize.y) && (pos.x >= 0) && (pos.y >= 0))
    {
      auto tile = tileID[this->tiles[pos.y*mapSize.x + pos.x].tile];
      return tile.canWalk;
    }
    return false;
  }

  world_t(glm::ivec2 mapSize)
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

  world_t(const world_t&) = delete;
  world_t& operator=(const world_t&) = delete;

  world_t(world_t&&) = delete;
  world_t& operator=(world_t&&) = delete;

  ~world_t() override
  {

  }

};

class starrg_t
{
  bb::context_t& context;
  bb::camera_t camera;
  bb::shader_t shader;
  bb::texture_t tileset;
  bb::mesh_t mesh;
  bb::mailbox_t::shared_t box;
  bb::actorPID_t world;

public:

  void Update(double dt)
  {
    bb::msg_t msg;
    while (this->box->Poll(&msg))
    {
      if (auto tileDesc = bb::As<bb::msg::dataMsg_t<bb::meshDesc_t>>(msg))
      {
        this->mesh = bb::GenerateMesh(
          tileDesc->Data()
        );
        continue;
      }
      // Unknown message!
      assert(0);
    }
    this->context.Title(std::to_string(1.0/dt));
  }

  void Render()
  {
    glDisable(GL_CULL_FACE);
    
    this->camera.Update();
    
    bb::shader_t::Bind(this->shader);
    this->shader.SetBlock("camera", camera.UniformBlock());

    bb::framebuffer_t::Bind(this->context.Canvas());
    bb::texture_t::Bind(tileset);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (this->mesh.Good())
    {
      this->mesh.Render();
    }
  }

  starrg_t(const starrg_t&) = delete;
  starrg_t(starrg_t&&) = delete;

  starrg_t& operator=(const starrg_t&) = delete;
  starrg_t& operator=(starrg_t&&) = delete;

  starrg_t(bb::context_t& context)
  : context(context),
    camera(bb::camera_t::Orthogonal(
      0.0f,
      static_cast<float>(context.Width()),
      static_cast<float>(context.Height()),
      0.0f)
    ),
    shader(bb::shader_t::LoadProgramFromFiles("starrg.vp.glsl", "starrg.fp.glsl")),
    tileset(bb::texture_t::LoadConfig("tiles.config")),
    box(bb::postOffice_t::Instance().New("StarView")),
    world(bb::workerPool_t::Instance().Register<world_t>(glm::ivec2(context.Width()/tileSize.x, context.Height()/tileSize.y)))
  {
    this->context.RegisterActorCallback(
      this->world,
      bb::context_t::keyboard
    );
  }

  ~starrg_t()
  {
    bb::workerPool_t::Instance().Unregister(this->world);
  }
};




int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return EXIT_FAILURE;
  }

  auto& context = bb::context_t::Instance();
  bb::workerPool_t::Instance();
  auto loop = true;
  bb::msg_t msg;
  auto lastTick = glfwGetTime();
  starrg_t starrg(context);

  while(loop)
  {
    auto nowTick = glfwGetTime();
    starrg.Update(nowTick - lastTick);
    starrg.Render();

    if (!context.Update())
    {
      break;
    }

    if (Mailbox()->Poll(&msg))
    {
      if (bb::As<bb::msg::poison_t>(msg))
      {
        loop = false;
        continue;
      }

      if (auto* defMsg = msg.get())
      {
        bb::Error("Unknown action type: %s", typeid(*defMsg).name());
      }
      else
      {
        bb::Error("Unknown action type: %s", "Invalid Message");
      }
      loop = false;
      assert(0);
    }

    lastTick = nowTick;
  }

  return EXIT_SUCCESS;
}