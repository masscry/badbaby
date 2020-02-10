project(painter)

add_executable(painter
# Headers
  include/painter.hpp

# Sources
  src/painter.cpp
)

target_include_directories(painter
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(painter
  PRIVATE
    config
    render
    actor
    simplex
    shapes
    effects
)
