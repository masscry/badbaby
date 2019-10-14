cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

project(badbaby
  VERSION 0.1
  LANGUAGES C CXX
)

find_package(Threads REQUIRED)

add_library(badbaby INTERFACE)

target_compile_features(badbaby INTERFACE
  cxx_std_11
  c_std_99
)

target_compile_options(badbaby INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror -fdiagnostics-color=always>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror -fcolor-diagnostics>
)

target_link_libraries(badbaby INTERFACE
  Threads::Threads
)

add_subdirectory(core)
add_subdirectory(tests)
add_subdirectory(atil)
