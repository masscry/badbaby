project(sub3000)

add_executable(sub3000
# Headers
  include/sub3000.hpp
  include/scene.hpp
  include/splash.hpp
  include/mainMenu.hpp
  include/actionTable.hpp
  include/authors.hpp
  include/arena.hpp
  include/space.hpp
  include/demo.hpp

# Sources
  src/sub3000.cpp
  src/scene.cpp
  src/actionTable.cpp

  src/scenes/mainMenu.cpp
  src/scenes/splash.cpp
  src/scenes/authors.cpp
  src/scenes/arena.cpp
  src/scenes/demo.cpp

  src/scenes/arena/player.cpp
  src/scenes/arena/space.cpp
  src/scenes/arena/screen_t.cpp
  src/scenes/arena/status_t.cpp
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
    effects
    mapgen
    sound
)

set_target_properties(sub3000 PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/sub3000"
)

install(TARGETS sub3000
  RUNTIME DESTINATION bin
  CONFIGURATIONS Release
)
