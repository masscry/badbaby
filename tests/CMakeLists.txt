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

target_link_libraries(003font PRIVATE config render shapes)

add_executable(004camera
  src/004camera.cpp
)

target_link_libraries(004camera PRIVATE config render shapes)

add_executable(005splash
  src/005splash.cpp
)

target_link_libraries(005splash PRIVATE config render)

add_executable(006simplex
  src/006simplex.cpp
)

target_link_libraries(006simplex PRIVATE config render simplex shapes)


add_executable(007menu
  src/007menu.cpp
)

target_link_libraries(007menu PRIVATE config render actor shapes)

add_executable(008bson
  src/008bson.cpp
)

target_link_libraries(008bson PRIVATE config)

install(TARGETS 
    000hello
    001render
    002config
    003font
    004camera
    005splash
    006simplex
    007menu
    008bson
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)