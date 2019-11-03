add_library(shapes 
# HEADERS
  include/shapes.hpp

# SOURCES
  src/shapes.cpp
)

target_link_libraries(shapes PUBLIC render)
target_include_directories(shapes PUBLIC include)
