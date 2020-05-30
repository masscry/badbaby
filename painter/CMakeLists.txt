project(painter)

add_executable(painter
# Headers
  include/painter.hpp

# Sources
  src/painter.cpp
)

target_include_directories(painter
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(painter
  PRIVATE
    config
    render
    actor
    shapes
    script
)

set_target_properties(painter PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/painter"
)
