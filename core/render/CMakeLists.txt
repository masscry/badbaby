add_subdirectory(contrib)

add_library(render STATIC
# HEADERS
  include/framebuffer.hpp
  include/context.hpp

# SOURCES
  src/framebuffer.cpp
  src/context.cpp
)

target_include_directories(render PUBLIC include)

target_link_libraries(render PUBLIC common glad glfw)
