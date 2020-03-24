add_executable(000hello
  src/000hello.cpp
)

target_link_libraries(000hello PRIVATE actor)

set_target_properties(000hello PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

add_executable(001render
  src/001render.cpp
)

target_link_libraries(001render PRIVATE actor render)

set_target_properties(001render PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

add_executable(002config
  src/002config.cpp
)

target_link_libraries(002config PRIVATE config)

set_target_properties(002config PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

add_executable(003font
  src/003font.cpp
)

target_link_libraries(003font PRIVATE config render shapes)

set_target_properties(003font PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

add_executable(004camera
  src/004camera.cpp
)

target_link_libraries(004camera PRIVATE config render shapes)

set_target_properties(004camera PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

add_executable(005splash
  src/005splash.cpp
)

target_link_libraries(005splash PRIVATE config render)

set_target_properties(005splash PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

add_executable(006simplex
  src/006simplex.cpp
)

target_link_libraries(006simplex PRIVATE config render simplex shapes mapgen )

set_target_properties(006simplex PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

add_executable(007menu
  src/007menu.cpp
)

target_link_libraries(007menu PRIVATE config render actor shapes)

set_target_properties(007menu PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

add_executable(008script
  src/008script.cpp
)

target_link_libraries(008script PRIVATE script)

set_target_properties(008script PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)
