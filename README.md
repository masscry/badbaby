# BadBaby Game Engine

BadBaby - simple game engine with OpenGL backend, actor based multithreading 
model.

## Project directory structure

* sub3000 - Game built around BadBaby engine
  * WORK IN PROGRESS
* core - engine core components
  * actor - actor-based multithreading system
  * common - common routines, like logging, UTF-8 etc.
  * config - simple config file parser/printer
  * render - OpenGL based renderer
  * util - utility libraries
    * shapes - rotuines and classes uses core.render to create objects to render
    * simplex - 3D OpenSimplex noise implementation 
* runtime - working directory for examples, resources stored here
* tests - engine feature test programs

## Compile&Run

1. *bootstrap.sh*
2. *cmake --build ./.debug --target {XXX}* where *{XXX}* test application from *tests* directory
3. *cd runtime; ../.debug/tests/{XXX}* where *{XXX}* test application to run

## Dependencies

* C++11
* C99
* OpenGL 3.3
* Flex 2.6.4
* Python 3.6
* glfw3 3.2
* glm 0.9.9
