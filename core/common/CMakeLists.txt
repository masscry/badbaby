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
  src/thread.cpp
  src/deci.cpp
)

if (BB_LINUX)
  list(APPEND COMMON_SOURCES src/monfs.cpp)
endif(BB_LINUX)

if(BB_APPLE)
  list(APPEND COMMON_SOURCES src/mac/monfs.cpp)
endif(BB_APPLE)

if(BB_WINDOWS)
  list(APPEND COMMON_SOURCES
	src/win/monfs.cpp
	src/win/getopt.cpp
	src/win/vasprintf.cpp
  )	
endif(BB_WINDOWS)

add_library(common STATIC
  ${COMMON_SOURCES}
)

target_include_directories(common PUBLIC include)
target_link_libraries(common PUBLIC badbaby)
