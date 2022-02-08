cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

project(badbaby
  VERSION 0.4.0
  LANGUAGES C CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

if(WIN32)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
else(WIN32)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
endif(WIN32)

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  if (CMAKE_VERSION VERSION_LESS 3.11)
    message(FATAL_ERROR
      "macOS depends on iconv detection, which added in CMAKE 3.11"
    )
  endif (CMAKE_VERSION VERSION_LESS 3.11)
  set(BB_APPLE TRUE)

  message(STATUS "Platform: macOS")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(STATUS "Platform: Linux")
  set(BB_LINUX TRUE)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(STATUS "Platform: Linux")
  set(BB_LINUX TRUE)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message(STATUS "Platform: Windows")
  set(BB_WINDOWS TRUE)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  option(BUILD_SHARED_LIBS "Build shared libs" OFF)
else (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  option(BUILD_SHARED_LIBS "Build shared libs" ON)
endif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")

find_package(Threads REQUIRED)

add_library(badbaby INTERFACE)

target_compile_options(badbaby INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wconversion -fdiagnostics-color=always>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wconversion -fcolor-diagnostics>
  $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wextra -Wconversion -fcolor-diagnostics -arch x86_64>
)

if (BB_WINDOWS)
	target_compile_definitions(badbaby INTERFACE
		_USE_MATH_DEFINES
		_CRT_SECURE_NO_WARNINGS
  )
  
  target_compile_options(badbaby INTERFACE
    /source-charset:utf-8 
  )
  
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  
endif(BB_WINDOWS)

target_link_libraries(badbaby INTERFACE
  Threads::Threads
)

# Additional path to search for glfw
if(UNIX)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif(UNIX)

add_subdirectory(core)
add_subdirectory(extra)
add_subdirectory(sound)
add_subdirectory(tests)
add_subdirectory(sub3000)
add_subdirectory(painter)
add_subdirectory(orthofight)
add_subdirectory(obj2mesh)
add_subdirectory(cqc)
add_subdirectory(star.rg)
add_subdirectory(tac.war)

install(DIRECTORY runtime/sub3000
  DESTINATION share
  CONFIGURATIONS Release
  FILES_MATCHING
    PATTERN "*.config"
    PATTERN "*.glsl"
    PATTERN "*.ipsum"
    PATTERN "*.tga"
    PATTERN "*.msh"
    PATTERN "*.ogg"
    PATTERN "*.wav"
    PATTERN "*.bbw"
)

if (UNIX)
install(FILES sub3000.sh
  DESTINATION ./
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  CONFIGURATIONS Release
)
endif(UNIX)

if (WIN32)
install(FILES sub3000.bat
  DESTINATION ./
  CONFIGURATIONS Release
)
endif(WIN32)

set(CPACK_PACKAGE_DESCRIPTION "Badbaby Demos")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VENDOR "deci.space")
set(CPACK_PACKAGE_CONTACT "masscry@deci.space")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CHECKSUM SHA256)
if(WIN32)
set(CPACK_GENERATOR ZIP)
else(WIN32)
set(CPACK_GENERATOR TGZ)
endif(WIN32)
set(CPACK_MONOLITHIC_INSTALL ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_DISPLAY_NAME "Badbaby Demos")

include (CPack)
