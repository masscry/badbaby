find_package(PythonInterp 3.6 REQUIRED)

execute_process(
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/glad/prepare.sh" ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/GLAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  RESULT_VARIABLE GLAD_PREP_FAILED
)

if (GLAD_PREP_FAILED)
  message(FATAL_ERROR "GLAD Preparations Failed!")
else(GLAD_PREP_FAILED)
  message(STATUS "GLAD Ready")
endif(GLAD_PREP_FAILED)

add_library(glad STATIC
# HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/GLAD/include/glad/glad.h
  ${CMAKE_CURRENT_BINARY_DIR}/GLAD/include/KHR/khrplatform.h

# SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/GLAD/src/glad.c
)

target_include_directories(glad PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}/GLAD/include
)

find_package(glfw3 3.2 REQUIRED)
