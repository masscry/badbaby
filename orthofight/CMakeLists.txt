project(orthofight)

add_executable(orthofight
# Headers
  

# Sources
  src/entry.cpp
)

target_include_directories(orthofight
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(orthofight
  PRIVATE
    config
    render
    actor
    simplex
    shapes
    effects
    mapgen
)

set_target_properties(orthofight PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/orthofight"
)

install(TARGETS orthofight
  RUNTIME DESTINATION bin
  CONFIGURATIONS Release
)
