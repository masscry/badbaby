#pragma once
#ifndef WORLD_HEADER
#define WORLD_HEADER

class world_t: public bb::role_t
{
  glm::ivec2 mapSize;
  std::vector<cell_t> tiles;
  std::deque<unit_t> units;
  std::unordered_multimap<glm::ivec2, std::deque<unit_t>::iterator, ivecKey_t, ivecKey_t> unitsOnMap;
  double timePassed;

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