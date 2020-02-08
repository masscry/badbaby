add_library(effects 
# HEADERS
  include/blur.hpp

# SOURCES
  src/blur.cpp
)

target_link_libraries(effects PUBLIC render shapes)
target_include_directories(effects PUBLIC include)
