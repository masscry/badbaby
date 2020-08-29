project(starrg)

add_executable(starrg
# Headers
  include/starrg.hpp
  include/tileDB.hpp
  include/ecs.hpp
  include/components.hpp
# Sources
  src/starrg.cpp
  src/tileDB.cpp
  src/entry.cpp
  src/world.cpp
  src/ecs.cpp
)

target_include_directories(starrg
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(starrg
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

set_target_properties(starrg PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/starrg"
)

install(TARGETS starrg
  RUNTIME DESTINATION bin
  CONFIGURATIONS Release
)
