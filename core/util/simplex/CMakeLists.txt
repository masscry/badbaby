add_library(simplex
# Headers
  include/simplex.hpp
# Sources
  src/simplex.cpp
)

target_include_directories(simplex PUBLIC include)

target_link_libraries(simplex PUBLIC badbaby common)
