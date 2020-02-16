add_library(shapes 
# HEADERS
  include/shapes.hpp
  include/text.hpp
  include/meshDesc.hpp
  include/vertexBuffer.hpp

# SOURCES
  src/shapes.cpp
  src/text.cpp
  src/meshDesc.cpp
)

target_link_libraries(shapes PUBLIC render)
target_include_directories(shapes PUBLIC include)
