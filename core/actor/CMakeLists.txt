
add_library(actor STATIC
# HEADERS
  include/worker.hpp
  include/actor.hpp
  include/mailbox.hpp
  include/role.hpp

# SOURCES
  src/worker.cpp
  src/actor.cpp
  src/mailbox.cpp
  src/role.cpp
)

target_include_directories(actor PUBLIC include)
target_link_libraries(actor PUBLIC badbaby common config render)
