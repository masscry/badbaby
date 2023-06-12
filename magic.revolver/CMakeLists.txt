project(magrev)

add_executable(magrev
# HEADERS
  include/tacwar.hpp
  include/tacmenu.hpp
  include/tacgame.hpp

# SOURCES
  src/tacwar.cpp
  src/tacmenu.cpp
  src/tacgame.cpp
)

target_include_directories(magrev
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(magrev
  PRIVATE
    config
    render
    actor
    simplex
    shapes
    effects
    mapgen
    sound
    script
)

set_target_properties(magrev PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/magrev"
)

install(TARGETS magrev
  RUNTIME DESTINATION bin
  CONFIGURATIONS Release
)
