add_executable(000hello
  src/000hello.cpp
)

target_link_libraries(000hello PRIVATE actor)

add_executable(001render
  src/001render.cpp
)

target_link_libraries(001render PRIVATE actor render)

add_executable(002config
  src/002config.cpp
)

target_link_libraries(002config PRIVATE config)
