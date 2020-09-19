#include <starrg.hpp>

const tileInfo_t tileID[] = {
  { { 0, 0 }, false, true , false }, // T_EMPTY
  { { 5, 0 }, false, true , false }, // T_GRASS
  { { 3, 0 }, false, true , false }, // T_ROCK
  { { 5, 0 }, true , true , false }, // T_GRASS_FLIP
  { { 3, 0 }, true , true , false }, // T_ROCK_FLIP
  { { 0, 1 }, false, false, false }, // T_TREE_0
  { { 1, 1 }, false, false, false }, // T_TREE_1
  { { 2, 1 }, false, false, false }, // T_TREE_2
  { { 3, 6 }, false, true , true  }, // T_LADDER
};

static_assert(bb::countof(tileID) == T_TOTAL, "All tiles must have IDs");
