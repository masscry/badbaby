cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

project(badbaby
  VERSION 0.1.0
  LANGUAGES C CXX
)

find_package(Threads REQUIRED)

add_library(badbaby INTERFACE)

target_compile_options(badbaby INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror -fdiagnostics-color=always>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror -fcolor-diagnostics>
)

target_link_libraries(badbaby INTERFACE
  Threads::Threads
)

add_subdirectory(core)
add_subdirectory(tests)
add_subdirectory(sub3000)

install(DIRECTORY runtime
  DESTINATION share
)

set(CPACK_PACKAGE_DESCRIPTION "Badbaby Examples")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VENDOR "deci.space")
set(CPACK_PACKAGE_CONTACT "masscry@deci.space")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

include (CPack)
