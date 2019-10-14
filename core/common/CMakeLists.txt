add_library(common
# HEADERS
  include/common.hpp

# SOURCES
  src/common.cpp
)

target_include_directories(common PUBLIC include)
target_link_libraries(common PUBLIC badbaby)
