add_library(script
# HEADERS
  include/script.hpp

# SOURCES
  src/script.cpp
)

target_include_directories(script PUBLIC include)
target_link_libraries(script PUBLIC config)

