project(mapgen)

add_library(mapgen STATIC
  include/mapGen.hpp
  include/heightMap.hpp
  src/mapGen.cpp
  src/heightMap.cpp
)

target_include_directories(mapgen PUBLIC include)
target_link_libraries(mapgen PUBLIC
  actor
  simplex
  render
)
