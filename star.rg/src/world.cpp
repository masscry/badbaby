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
    std::unordered_multimap<glm::ivec2, unit_t, ivecKey_t, ivecKey_t> newMapUnits;
    sr::pos_t::factory_t::Instance().Each(
      [&newMapUnits](sr::pos_t& pos)
      {
        newMapUnits.emplace(
          pos.Data().v,
          pos.ID()
        );
      }
    );
    this->unitsOnMap = std::move(newMapUnits);
  }
}

const sr::spriteData_t pikeManSprite = { {26, 0} };

const sr::meleeData_t pikeManMelee = {
  80, // int skill; // probability to hit
  60, // int stren; // hit strenght
  60, // int tough; // unit toughness
  50  // int armor; // armor rating
};

const sr::updateData_t pikeManUpdate = {
  1000,
  0
};

const sr::spriteData_t orkSprite = {
  {26, 2}
};

const sr::meleeData_t orkMelee = {
  20, // int skill; // probability to hit
  90, // int stren; // hit strenght
  50, // int tough; // unit toughness
  5   // int armor; // armor rating
};

const sr::updateData_t orkUpdate = {
  2000,
  0
};

const sr::aiData_t orkAI = {
  sr::AI_STALKER
};

void world_t::GenerateMap()
{
  auto &engine = RandomEngine();

  this->tiles.clear();
  this->tiles.resize(
    static_cast<size_t>(this->mapSize.x * this->mapSize.y),
    cell_t{T_EMPTY, false, false}
  );

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

  while (this->CanStand(plPos) == false)
  {
    plPos.x += jumpDist(engine) - 3;
    plPos.y += jumpDist(engine) - 3;
    if ((plPos.x < 0) || (plPos.x > this->mapSize.x-1) || (plPos.y < 0) || (plPos.y > this->mapSize.y-1))
    {
      plPos.x = posXDist(engine);
      plPos.y = posYDist(engine);
    }
  }

  this->units.clear();
  sr::entityFactory_t::Instance().Clear();

  auto playerEntity = sr::entityFactory_t::Instance().GetNew();

  sr::pos_t::factory_t::Instance().NewComponent(
    playerEntity,
    { plPos }
  );

  sr::sprite_t::factory_t::Instance().NewComponent(
    playerEntity,
    pikeManSprite
  );

  sr::melee_t::factory_t::Instance().NewComponent(
    playerEntity,
    pikeManMelee
  );

  sr::update_t::factory_t::Instance().NewComponent(
    playerEntity,
    pikeManUpdate
  );

  sr::status_t::factory_t::Instance().NewComponent(
    playerEntity,
    {
      sr::US_NONE,
      glm::vec2(0.0f)
    }
  );

  sr::userInput_t::factory_t::Instance().NewComponent(
    playerEntity,
    {
      sr::SIDE_TOTAL,
      0
    }
  );

  this->units.emplace_back(playerEntity);

  for (int i = 0; i < 10; ++i)
  {
    auto pos = glm::ivec2{
      posXDist(engine),
      posYDist(engine)
    };

    while (this->CanStand(pos) == false)
    {
      pos.x += jumpDist(engine) - 3;
      pos.y += jumpDist(engine) - 3;
      if ((pos.x < 0) || (pos.x > this->mapSize.x-1) || (pos.y < 0) || (pos.y > this->mapSize.y-1))
      {
        pos.x = posXDist(engine);
        pos.y = posYDist(engine);
      }
    }

    auto orkEntity = sr::entityFactory_t::Instance().GetNew();

    sr::pos_t::factory_t::Instance().NewComponent(
      orkEntity,
      { pos }
    );

    sr::sprite_t::factory_t::Instance().NewComponent(
      orkEntity,
      orkSprite
    );

    sr::melee_t::factory_t::Instance().NewComponent(
      orkEntity,
      orkMelee
    );

    sr::update_t::factory_t::Instance().NewComponent(
      orkEntity,
      orkUpdate
    );

    sr::ai_t::factory_t::Instance().NewComponent(
      orkEntity,
      orkAI
    );

    sr::status_t::factory_t::Instance().NewComponent(
      orkEntity,
      {
        sr::US_NONE,
        glm::vec2(0.0f)
      }
    );

    this->units.emplace_back(orkEntity);
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
      float fdx = static_cast<float>(dx);
      float fdy = static_cast<float>(dy);
      auto l_slope = (fdx - 0.5f) / (fdy + 0.5f);
      auto r_slope = (fdx + 0.5f) / (fdy - 0.5f);
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
      if ((sax < 0 && std::abs(sax) > pos.x) ||
          (say < 0 && std::abs(say) > pos.y))
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
        if (!this->CanStand(glm::ivec2{ax, ay}))
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
      else if (!this->CanStand(glm::ivec2{ax, ay}))
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
    auto& unitPosComponent = sr::pos_t::factory_t::Instance().Item(unit);
    auto& unitSpriteComponent = sr::sprite_t::factory_t::Instance().Item(unit);
    auto& unitStatusComponent = sr::status_t::factory_t::Instance().Item(unit);

    auto unitPos = unitPosComponent.Data().v;
    auto& unitStatus = unitStatusComponent.Data();

    auto info = this->Tiles(unitPos);

    if (!info.visible)
    {
      continue;
    }

    glm::vec2 pos = {
      static_cast<float>(unitPos.x) * tileSize.x,
      static_cast<float>(unitPos.y) * tileSize.y
    };

    if (indOffset + 4 >= UINT16_MAX)
    { // Invalid offset!
      BB_PANIC();
      continue;
    }

    quadData_t q = CreateQuad(unitSpriteComponent.Data().id, pos, glm::vec3(1.0f), static_cast<uint16_t>(indOffset), false);
    for (auto& v: q.vPos)
    {
      v.z = 1.0f;
    }

    switch (unitStatus.status)
    {
    case sr::US_NONE:
      break;
    case sr::US_MISS:
      for (auto& v: q.vShim)
      {
        v = unitStatus.side*10.0f;
      }
      unitStatus.status = sr::US_NONE;
      break;
    case sr::US_ARMOR:
      {
        if (indOffset + 4 >= UINT16_MAX)
        {
          BB_PANIC();
        }
        else
        {
          quadData_t shield = CreateQuad(
            glm::ivec2{39, 3},
            pos,
            glm::vec3(1.0f),
            static_cast<uint16_t>(indOffset+4),
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
            v = unitStatus.side*14.0f;
          }
          vec.Add(shield);
          indOffset += 4;
        }
        unitStatus.status = sr::US_NONE;
      }
      break;
    case sr::US_SAVE:
      for (auto& v: q.vShim)
      {
        v = glm::vec2(unitStatus.side.y, -unitStatus.side.x)*10.0f;
      }
      unitStatus.status = sr::US_NONE;
      break;
    case sr::US_DEAD:
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
      BB_PANIC();
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

  if (mapSize.y * mapSize.x < 0)
  { // Invalid map dimensions!
    BB_PANIC();
    return bb::meshDesc_t();
  }

  if (!this->units.empty())
  {
    playerPos = sr::pos_t::factory_t::Instance().Item(this->units[0]).Data().v;
  }

  vec.pos.reserve(static_cast<size_t>(mapSize.y * mapSize.x * 4));
  vec.uv.reserve(static_cast<size_t>(mapSize.y * mapSize.x * 4));
  vec.col.reserve(static_cast<size_t>(mapSize.y * mapSize.x * 4));
  vec.shim.reserve(static_cast<size_t>(mapSize.y * mapSize.x * 4));
  vec.ind.reserve(static_cast<size_t>(mapSize.y * mapSize.x * 6));

  int indOffset = 0;

  for (auto &cell : this->tiles)
  {
    cell.shadow = cell.visible || cell.shadow;
    cell.visible = false;
  }

  this->Tiles(playerPos).visible = true;

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
      glm::vec2 pos = {
        static_cast<float>(x) * tileSize.x,
        static_cast<float>(y) * tileSize.y
      };

      if (indOffset + 4 >= UINT16_MAX)
      { // Too many indecies!
        BB_PANIC();
        continue;
      }

      quadData_t q = CreateQuad(tile.pos, pos, color, static_cast<uint16_t>(indOffset), tile.flip);

      vec.Add(q);
      indOffset += 4;
    }
  }

  return vec.Create();
}

void world_t::ProcessAI()
{
  // Обработка ответа мира на действия игрока
  auto &engine = RandomEngine();
  std::uniform_int_distribution<int> modeSide(sr::SIDE_UP, sr::SIDE_UP_LEFT);
  for (auto unit: this->units)
  {
    auto& unitStatus = sr::status_t::factory_t::Instance().Item(unit);
    if (unitStatus->status == sr::US_DEAD)
    {
      continue;
    }

    auto& unitUpdate = sr::update_t::factory_t::Instance().Item(unit);

    unitUpdate->curTime += this->timePassed;
    while (unitUpdate->curTime > unitUpdate->moveTime)
    {
      unitUpdate->curTime -= unitUpdate->moveTime;
      auto* unitAI = sr::ai_t::factory_t::Instance().OptionalItem(unit);
      if (unitAI == nullptr)
      {
        continue;
      }

      switch(unitAI->Data().ai)
      {
        case sr::AI_STALKER:
        {
          // Каждые два хода игрока, все орки двигаются в случайном направлении
          auto& unitPos = sr::pos_t::factory_t::Instance().Item(unit);
          auto side = (sr::unitSide_t) modeSide(engine);
          if (this->CanWalk(unitPos->v, side))
          {
            unitPos->v += sr::iSideVec[side];
          }
          break;
        }
        case sr::AI_SCARED:
          break;
        case sr::AI_ANGRY:
          break;
        default:
          BB_PANIC();
      }
    }
  }
  this->timePassed = 0;
}

bb::msg::result_t world_t::OnKeyPress(const bb::msg::keyEvent_t& key)
{
  switch (key.Press())
  {
    case GLFW_RELEASE:
    {
      if (key.Key() == GLFW_KEY_ESCAPE)
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
      switch (key.Key())
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
              action_t{key.Key()},
              -1));
          break;
        default:
          break;
      }
    }
    break;
    default:
      BB_PANIC();
      break;
  }
  return bb::msg::result_t::complete;
}

static const uint32_t SQ2 = 1414;
static const uint32_t SQ1 = 1000;

bb::msg::result_t world_t::OnProcessMessage(const bb::actor_t &, const bb::msg::basic_t &msg)
{
  if (auto key = bb::msg::As<bb::msg::keyEvent_t>(msg))
  {
    return this->OnKeyPress(*key);
  }

  if (auto action = bb::msg::As<bb::msg::dataMsg_t<action_t>>(msg))
  {
    uint32_t totalTimePassed = 0;

    sr::userInput_t::factory_t::Instance().Each(
      [this,action,&totalTimePassed](sr::userInput_t& input)
      {
        // обработка ввода игрока
        switch (action->Data().key)
        {
          case GLFW_KEY_KP_1:
            input->side = sr::SIDE_DOWN_LEFT;
            totalTimePassed += SQ2;
            ++input->steps;
            break;
          case GLFW_KEY_KP_2:
            input->side = sr::SIDE_DOWN;
            totalTimePassed += SQ1;
            ++input->steps;
            break;
          case GLFW_KEY_KP_3:
            input->side = sr::SIDE_DOWN_RIGHT;
            totalTimePassed += SQ2;
            ++input->steps;
            break;
          case GLFW_KEY_KP_4:
            input->side = sr::SIDE_LEFT;
            totalTimePassed += SQ1;
            ++input->steps;
            break;
          case GLFW_KEY_KP_6:
            input->side = sr::SIDE_RIGHT;
            totalTimePassed += SQ1;
            ++input->steps;
            break;
          case GLFW_KEY_KP_7:
            input->side = sr::SIDE_UP_LEFT;
            totalTimePassed += SQ2;
            ++input->steps;
            break;
          case GLFW_KEY_KP_8:
            input->side = sr::SIDE_UP;
            totalTimePassed += SQ1;
            ++input->steps;
            break;
          case GLFW_KEY_KP_9:
            input->side = sr::SIDE_UP_RIGHT;
            totalTimePassed += SQ2;
            ++input->steps;
            break;
          default:
            input->side = sr::SIDE_TOTAL;
        }

        if (input->side != sr::SIDE_TOTAL)
        {
          auto& playerPos = sr::pos_t::factory_t::Instance().Item(input.ID());

          // Если можно сделать ход
          if (this->CanWalk(playerPos->v, input->side))
          {
            playerPos->v += sr::iSideVec[input->side];

            auto tileInfo = this->TileInfo(playerPos->v);
            if (tileInfo.ladder)
            { // Специальная клетка - лестница
              this->GenerateMap();
            }
            else
            { // Двигаем остальных и обновляем карту
              this->ProcessAI();
              this->UpdateMapUnits();
            }

            // Отправляем новую карту
            bb::postOffice_t::Instance().Post(
              "StarView",
              bb::Issue<meshData_t>(
                this->BuildTileMap(),
                meshData_t::M_MAP
              )
            );

            // Отправляем новых юнитов
            bb::postOffice_t::Instance().Post(
              "StarView",
              bb::Issue<meshData_t>(
                this->BuildUnits(),
                meshData_t::M_UNIT
              )
            );
            // Больше делать нечего - заканчиваем
            return;
          }

          // Пройти не получилось, пробуем драться!
          auto targetPos = playerPos->v + sr::iSideVec[input->side];

          // Если в клетке есть враг - пытаемся драться
          auto target = this->unitsOnMap.find(targetPos);
          if (target != this->unitsOnMap.end())
          {
            auto& playerStatus = sr::status_t::factory_t::Instance().Item(this->units[0]);
            auto& targetStatus = sr::status_t::factory_t::Instance().Item(target->second);
            if (targetStatus->status != sr::US_DEAD)
            {
              std::stringstream logst;
              auto &engine = RandomEngine();
              std::uniform_int_distribution<int> dice(0, 100);

              auto& playerMelee = sr::melee_t::factory_t::Instance().Item(this->units[0]);
              auto& targetMelee = sr::melee_t::factory_t::Instance().Item(target->second);
              targetStatus->side = sr::sideVec[input->side];

              logst << "[" << input->steps << "]" << "Конан бьёт! ";

              // Как хорошо попадаем
              int hitRoll = dice(engine);
              if (hitRoll <= playerMelee->skill)
              {
                // Чтобы ранить - надо пробить броню своей силой
                int woundDifficulty = 50; // Если равны - то вероятность 50/50

                woundDifficulty += (playerMelee->stren > targetMelee->tough)*20; // Если сила выше, то +20%
                woundDifficulty += (playerMelee->stren > targetMelee->tough*2)*20; // Если в два раза выше +40%
                woundDifficulty -= (playerMelee->stren < targetMelee->tough)*20; // Если сила меньше, то -20%
                woundDifficulty -= (playerMelee->stren*2 < targetMelee->tough)*20; // Если в два раза меньше -40%

                int woundRoll = dice(engine);

                if (woundRoll <= woundDifficulty)
                { // Рана попала
                  
                  // Воин способен минимизировать вред от раны
                  int armorSave = dice(engine);
                  if (armorSave > targetMelee->armor)
                  { // Не вышло!
                    logst << "Убил!";
                    targetStatus->status = sr::US_DEAD;
                  }
                  else
                  { // Рана не прошла
                    logst << "Не задел! (S" << armorSave << "<" << targetMelee->armor << ')';
                    playerStatus->status = sr::US_SAVE;
                    playerStatus->side = -sr::sideVec[input->side];
                    targetStatus->status = sr::US_SAVE;
                  }
                }
                else
                { // Спасла броня
                  logst << "Мечь отскочил от брони! (W" << woundRoll << ">" << woundDifficulty << ')';
                  playerStatus->status = sr::US_MISS;
                  playerStatus->side = -sr::sideVec[input->side];
                  targetStatus->status = sr::US_ARMOR;
                }
              }
              else
              { // Вообще не попал
                logst << "Промазал! (H" << hitRoll << ">" << playerMelee->skill << ')';
                playerStatus->status = sr::US_MISS;
                playerStatus->side = -sr::sideVec[input->side];
                targetStatus->status = sr::US_MISS;
              }

              // Отправка сообщения в лог
              bb::postOffice_t::Instance().Post(
                "StarView",
                bb::Issue<bb::msg::dataMsg_t<std::string>>(
                  logst.str(),
                  -1
                )
              );
            }
          }
        }
      }
    );

    this->timePassed += totalTimePassed;

    // Двигаем кого можем и обновляем карту
    this->ProcessAI();
    this->UpdateMapUnits();

    // Отправляем новые положения и состояния юнитов
    bb::postOffice_t::Instance().Post(
      "StarView",
      bb::Issue<meshData_t>(
        this->BuildUnits(),
        meshData_t::M_UNIT
      )
    );

    return bb::msg::result_t::complete;
  }

  // unknown message
  BB_PANIC();
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

bool world_t::CanStand(glm::ivec2 pos) const
{
  if ((pos.x < this->mapSize.x) && (pos.y < this->mapSize.y) && (pos.x >= 0) && (pos.y >= 0))
  {
    auto tile = tileID[this->Tiles(pos).tile];
    auto unit = this->unitsOnMap.find(pos) ;
    if ((unit == this->unitsOnMap.end()) ||  (sr::status_t::factory_t::Instance().Item(unit->second)->status == sr::US_DEAD))
    {
      return tile.canWalk;
    }
  }
  return false;
}

bool world_t::CanWalk(glm::ivec2 pos, sr::unitSide_t side) const
{
  auto newPos = pos + sr::iSideVec[side];
  if (this->CanStand(newPos) == false)
  {
    return false;
  }

  if ((newPos.x != pos.x) && (newPos.y != pos.y))
  { // Если движение по диагонали, то надо проверить, можно ли пройти 
    // также в стороны, если хоть в одну из сторон пройти нельзя - 
    // то пройти нельзя

    if ( (this->CanStand({ pos.x, newPos.y }) == false) 
      && (this->CanStand({ newPos.x, pos.y }) == false))
    {
      return false;
    }
  }

  return true;
}

world_t::world_t(glm::ivec2 mapSize)
: mapSize(mapSize)
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
