macro(SETUP_TEST TEST_NAME)

add_executable(${TEST_NAME}
  src/${TEST_NAME}.cpp
)

target_link_libraries(${TEST_NAME}
	PRIVATE
		actor
		render
		config
		shapes
		simplex
		mapgen
		script
		binstore
)

set_target_properties(${TEST_NAME} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_SOURCE_DIR}/runtime/tests"
)

endmacro(SETUP_TEST)

SETUP_TEST(001render)
SETUP_TEST(002config)
SETUP_TEST(003font)
SETUP_TEST(004camera)
SETUP_TEST(005splash)
SETUP_TEST(006simplex)
SETUP_TEST(007menu)
SETUP_TEST(008script)
SETUP_TEST(009field)
SETUP_TEST(010bin)
SETUP_TEST(011automata)
