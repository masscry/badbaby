add_library(config STATIC
# HEADERS 
  include/config.hpp
  include/value.hpp

# SOURCES
  src/config.cpp
  src/value.cpp
)

target_include_directories(config PUBLIC include)
target_link_libraries(config PUBLIC common)
