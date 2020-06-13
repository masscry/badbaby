project(obj2mesh)

set (OBJ2MESH_SOURCES
  src/obj2mesh.cpp
)

if(WIN32)
set(OBJ2MESH_PLATFORM_SOURCES
  include/getline.hpp
  src/getline.cpp
)
endif(WIN32)

add_executable(obj2mesh
  ${OBJ2MESH_SOURCES}
  ${OBJ2MESH_PLATFORM_SOURCES}
)

target_include_directories(obj2mesh PRIVATE
  include
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
