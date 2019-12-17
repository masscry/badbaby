project(sub3000)

add_executable(sub3000
# Headers
  include/sub3000.hpp
  include/scene.hpp
  include/splash.hpp
  include/mainMenu.hpp

# Sources
  src/sub3000.cpp
  src/scene.cpp
  src/splash.cpp
  src/mainMenu.cpp
)

target_include_directories(sub3000
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(sub3000
  PRIVATE
    config
    render
    actor
    simplex 
    shapes
)
