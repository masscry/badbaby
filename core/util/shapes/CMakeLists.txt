add_library(shapes STATIC
# HEADERS
  include/shapes.hpp
  include/text.hpp
  include/meshDesc.hpp
  include/vertexBuffer.hpp
  include/indexBuffer.hpp

# SOURCES
  src/shapes.cpp
  src/planes.cpp
  src/text.cpp
  src/meshDesc.cpp
  src/circle.cpp
  src/lines.cpp
  src/numbers.cpp
  src/vertexBuffer.cpp
  src/indexBuffer.cpp
  src/vecfont.cpp
)

target_link_libraries(shapes PUBLIC render)
target_include_directories(shapes PUBLIC include)
