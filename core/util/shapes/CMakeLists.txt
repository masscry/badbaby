add_library(shapes 
# HEADERS
  include/shapes.hpp
  include/text.hpp

# SOURCES
  src/shapes.cpp
  src/text.cpp
)

target_link_libraries(shapes PUBLIC render)
target_include_directories(shapes PUBLIC include)
