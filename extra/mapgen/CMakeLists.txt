project(mapgen)

add_library(mapgen STATIC
  include/mapGen.hpp
  src/mapGen.cpp
)

target_include_directories(mapgen PUBLIC include)
target_link_libraries(mapgen PUBLIC
  actor
  simplex
  render
)
