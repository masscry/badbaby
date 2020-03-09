set(COMMON_SOURCES
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
)

if (BB_LINUX)
  list(APPEND COMMON_SOURCES src/monfs.cpp)
elseif(BB_APPLE)
  list(APPEND COMMON_SOURCES src/mac/monfs.cpp)
endif(BB_LINUX)

add_library(common STATIC
  ${COMMON_SOURCES}
)

target_include_directories(common PUBLIC include)
target_link_libraries(common PUBLIC badbaby)
