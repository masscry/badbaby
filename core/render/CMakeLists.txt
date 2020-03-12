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

target_link_libraries(render PUBLIC common config glad glfw glm::glm ${GLFW_LIBRARIES} actor)

if (BB_APPLE)
  find_package(Iconv REQUIRED)
  target_link_libraries(render PUBLIC Iconv::Iconv)
endif(BB_APPLE)

if (BB_WINDOWS)
  target_link_libraries(render PUBLIC WinIconv::WinIconv)
endif(BB_WINDOWS)