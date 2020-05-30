project(binstore)

add_library(binstore STATIC
  include/binstore.hpp
  src/binstore.cpp
)

target_include_directories(binstore PUBLIC include)
target_link_libraries(binstore PUBLIC common)

if(WIN32)
target_link_libraries(binstore PUBLIC wsock32 ws2_32)
endif(WIN32)