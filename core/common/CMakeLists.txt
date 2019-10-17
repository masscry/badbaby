add_library(common STATIC
# HEADERS
  include/common.hpp
  include/image.hpp

# SOURCES
  src/common.cpp
  src/targa.cpp
  src/image.cpp
)

target_include_directories(common PUBLIC include)
target_link_libraries(common PUBLIC badbaby)
