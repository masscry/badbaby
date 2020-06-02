project(obj2mesh)

add_executable(obj2mesh
# Sources
  src/obj2mesh.cpp
)

target_link_libraries(obj2mesh
  PRIVATE
    config
    render
    actor
    shapes
    script
)

set_target_properties(obj2mesh PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/obj2mesh"
)
