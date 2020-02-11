add_library(common STATIC
# HEADERS
  include/common.hpp
  include/image.hpp
  include/utf8.hpp
  include/monfs.hpp

# SOURCES
  src/common.cpp
  src/targa.cpp
  src/image.cpp
  src/utf8.cpp
  src/monfs.cpp
)

target_include_directories(common PUBLIC include)
target_link_libraries(common PUBLIC badbaby)
