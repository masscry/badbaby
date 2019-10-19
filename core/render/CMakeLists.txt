add_subdirectory(contrib)

add_library(render STATIC
# HEADERS
  include/framebuffer.hpp
  include/context.hpp
  include/shader.hpp
  include/texture.hpp
  include/vao.hpp

# SOURCES
  src/framebuffer.cpp
  src/context.cpp
  src/shader.cpp
  src/texture.cpp
  src/vao.cpp
)

target_include_directories(render PUBLIC include)

target_link_libraries(render PUBLIC common config glad glfw)
