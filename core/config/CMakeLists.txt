find_package(FLEX 2.6.4 REQUIRED)

FLEX_TARGET(ConfigScanner src/config.lex ${CMAKE_CURRENT_BINARY_DIR}/config.lex.c
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/config.lex.h
)

add_library(config STATIC
# HEADERS 
  include/config.hpp
  include/value.hpp
  ${FLEX_ConfigScanner_OUTPUT_HEADER}

# SOURCES
  src/config.cpp
  src/value.cpp
  ${FLEX_ConfigScanner_OUTPUTS}
)

# add strtod_l support for C objects (aka config.lex.c)
if(UNIX)
  target_compile_definitions(config PRIVATE _GNU_SOURCE)
endif(UNIX)

target_include_directories(config
PUBLIC
  include
PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(config PUBLIC common)
