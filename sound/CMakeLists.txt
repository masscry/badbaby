project(sound)

add_library(bass SHARED IMPORTED)
set_target_properties(bass
  PROPERTIES
    IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/contrib/bass/x64/libbass.so
)

set_target_properties(bass
  PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/contrib/bass/x64/libbass.so
)

add_library(sound STATIC
  include/sound.hpp
  src/sound.cpp
)

target_include_directories(sound
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/contrib/bass
)

target_link_libraries(sound PUBLIC
  bass
  common
)
