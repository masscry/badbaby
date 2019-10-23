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

add_executable(003font
  src/003font.cpp
)

target_link_libraries(003font PRIVATE config render)

add_executable(004camera
  src/004camera.cpp
)

target_link_libraries(004camera PRIVATE config render)

install(TARGETS 
    000hello
    001render
    002config
    003font
    004camera
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)