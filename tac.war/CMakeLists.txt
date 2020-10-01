project(tacwar)

add_executable(tacwar
# HEADERS
  include/tacwar.hpp
  include/tacmenu.hpp
  include/tacgame.hpp

# SOURCES
  src/tacwar.cpp
  src/tacmenu.cpp
  src/tacgame.cpp
)

target_include_directories(tacwar
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tacwar
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

set_target_properties(tacwar PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tacwar"
)

install(TARGETS tacwar
  RUNTIME DESTINATION bin
  CONFIGURATIONS Release
)
