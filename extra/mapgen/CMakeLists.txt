project(mapgen)

add_library(mapgen STATIC
  include/mapGen.hpp
  include/heightMap.hpp
  include/distanceMap.hpp
  src/mapGen.cpp
  src/heightMap.cpp
  src/distanceMap.cpp
)

target_include_directories(mapgen PUBLIC include)
target_link_libraries(mapgen PUBLIC
  actor
  simplex
  render
  binstore
)
