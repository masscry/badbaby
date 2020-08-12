#pragma once
#ifndef WORLD_HEADER
#define WORLD_HEADER

class meshData_t: public bb::msg::dataMsg_t<bb::meshDesc_t>
{

public:

  enum type_t
  {
    M_MAP = 0,
    M_UNIT
  } type;

   meshData_t(bb::meshDesc_t&& data, type_t type)
   : bb::msg::dataMsg_t<bb::meshDesc_t>(std::move(data), -1),
     type(type)
   {
     ;
   }

};

class world_t: public bb::role_t
{
  glm::ivec2 mapSize;
  std::vector<cell_t> tiles;
  std::deque<unit_t> units;
  std::unordered_multimap<glm::ivec2, std::deque<unit_t>::iterator, ivecKey_t, ivecKey_t> unitsOnMap;
  double timePassed;
  uint64_t step;

  cell_t& Tiles(glm::ivec2 v)
  {
    return this->tiles[v.y * mapSize.x + v.x];
  }

  const cell_t& Tiles(glm::ivec2 v) const
  {
    return this->tiles[v.y * mapSize.x + v.x];
  }

  void UpdateMapUnits();
  void GenerateMap();

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
  );

  void UpdateFOV(glm::ivec2 pos, int radius);
  bb::meshDesc_t BuildTileMap();
  bb::meshDesc_t BuildUnits();

  bb::msg::result_t OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg) override;

public:

  const char* DefaultName() const override;

  tileInfo_t TileInfo(glm::ivec2 pos) const;

  bool CanWalk(glm::ivec2 pos) const;

  world_t(glm::ivec2 mapSize);

  world_t(const world_t&) = delete;
  world_t& operator=(const world_t&) = delete;

  world_t(world_t&&) = delete;
  world_t& operator=(world_t&&) = delete;

  ~world_t() override;

};

#endif /* WORLD_HEADER */