find_package(PythonInterp 3.6 REQUIRED)

if(WIN32)
# WIN32 Python3 places venv scripts in Script directory
# also programs have .exe suffix
set(VENV_BIN_DIR "Scripts")
set(VENV_EXE_SUFFIX ".exe")
else(WIN32)
# All other platforms but venv script in bin directory
set(VENV_BIN_DIR "bin")
set(VENV_EXE_SUFFIX "")
endif(WIN32)

add_custom_command(
  OUTPUT 
    "${CMAKE_CURRENT_BINARY_DIR}/GLAD/include/glad/glad.h"
    "${CMAKE_CURRENT_BINARY_DIR}/GLAD/include/KHR/khrplatform.h"
    "${CMAKE_CURRENT_BINARY_DIR}/GLAD/src/glad.c"
  COMMAND "${PYTHON_EXECUTABLE}" -m venv "${CMAKE_CURRENT_BINARY_DIR}/.pyenv"
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/.pyenv/${VENV_BIN_DIR}/pip${VENV_EXE_SUFFIX}" install wheel 
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/.pyenv/${VENV_BIN_DIR}/pip${VENV_EXE_SUFFIX}" install glad
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/.pyenv/${VENV_BIN_DIR}/glad${VENV_EXE_SUFFIX}" --profile core --out-path "${CMAKE_CURRENT_BINARY_DIR}/GLAD" --api gl=3.3 --generator c --spec gl --reproducible --no-loader
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT "Prepare GLAD"
)

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

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Do not build examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Do not build tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Do not build docs")
set(GLFW_INSTALL ON CACHE BOOL "Make install target")
set(GLFW_INSTALL_ONLY_LIBS ON CACHE BOOL "Install only runtime target")
set(GLFW_USE_WAYLAND OFF CACHE BOOL "Do not use Wayland")
set(GLFW_USE_MIR OFF CACHE BOOL "Do not use MIR")

add_subdirectory(glfw)

add_subdirectory(glm)

target_compile_definitions(glm INTERFACE
  GLM_FORCE_XYZW_ONLY
)

if(BB_WINDOWS)
	add_subdirectory(win-iconv)
endif(BB_WINDOWS)
