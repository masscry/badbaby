add_subdirectory(contrib)

add_library(render STATIC
# HEADERS
  include/framebuffer.hpp
  include/context.hpp
  include/shader.hpp
  include/texture.hpp
  include/vao.hpp
  include/font.hpp
  include/ubo.hpp
  include/camera.hpp

# SOURCES
  src/framebuffer.cpp
  src/context.cpp
  src/shader.cpp
  src/texture.cpp
  src/vao.cpp
  src/font.cpp
  src/ubo.cpp
  src/camera.cpp
)

target_include_directories(render PUBLIC include ${GLM_INCLUDE_DIRS})

target_link_libraries(render PUBLIC common config glad glfw)
