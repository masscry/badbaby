project(cqc)

add_executable(cqc
# Headers
  

# Sources
  src/cqc.cpp
)

target_include_directories(cqc
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cqc
  PRIVATE
    config
    render
    actor
    simplex
    shapes
    effects
    mapgen
)

set_target_properties(cqc PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/cqc"
)
