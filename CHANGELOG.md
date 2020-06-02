# Changelog

All notable changes to this project will be documented in this file

## [Unreleased]

## [0.2.1] - 2020-06-02

### Added
 - common: decimal numbers class added (deci_t)
 - render: floating point framebuffer support 
 - sub3000:
    * depth meter
    * ballast control
    * collisions
    * time scale
    * warning
    * control visuals (rudder, engine, ballast)
    * depth countor map
    * 'infinity' scrolling map
 - util: vector letters support
 - actor: template to box data in messages
 - painter: support for negative numbers and text

### Changed
 - painter: synchronize shader with one used in sub3000
 - sub3000: fix blending issues across different scenes
 - util: better lines and points, can be stored in single mesh.
 - win32:
     * warning fixes
     * fix postOffice early deletion bug
     * fix camera bug
     * fix map read bug
 - render: fix context multithreading issues with window title on Win32

### Removed
 - sub3000: status text


## [0.2.0] - 2020-05-21

### Added
 - extra.mapgen: map generation added
 - extra.binstore: simple de\serializer
 - tests: 008script, 009field, 010bin, 011automata added
 - orthofight: added
 - sub3000: radar raycasting implemented

### Changed
 - core.render.contrib: build glfw from sources
 - core.render.contrib: glm, glfw, win-iconv moved to submodules
 - core.render: bug fixes, big indecies > 0xFFFF support added
 - core.util.shapes: points now round, shader fixes
 - core: started porting to Windows
 - core: multiple bugs fixed
 - sub3000: physics finetuning

## [0.1.1] - 2020-03-10
### Changed
 - core: compiles on macOS

## [0.1.0] - 2020-03-06
### Added
 - core.shapes: textStatic_t typography, textDynamic_t multiline support
 - core.shapes: ring, line and point shapes
 - core.effects: simple blur shaded
 - core.render: camera_t added, UBO support
 - core: filesystem monitor based on inotify
 - sub3000: game prototype, simple control, radar screen
 
### Changed
 - core.render: better API for shader, VAO, framebuffer
 - core.render: meshes improved
 - clang, gcc: build fixes, warning resolving
 - core.actor: message system redone, actors redone
 - core.config: config block support added

### Removed
 - core.config: BSON removed


## [0.0.5] - 2019-11-17
### Added
- core.config: simple BSON parser
- extra: EditBSON application

## [0.0.4] - 2019-11-09
### Added
- tests: 007menu
- core.render: actor binding to callbacks
- core.actor: search actor ID by actor's name

### Changed
- plan: new game vision
- core.shapes: plane stack object removed debug output
- core.common: file_t replaced with BB_DEFER
- core.actor: msg_t is no POD, actor names replaced with actor IDs
- core.shapes: text classes moved here from core.render
- core.shapes: textDynamic_t buffer allocation bug fixed and optimized
- core.actor: message polling added to mailbox class

## [0.0.3] - 2019-11-07
### Added
- tests: 005splash, 006simplex
- libraries: core.util.shapes, core.util.simplex
- core.render.context: title text update routine

### Changed
- core.actor: total worker thread count: can be set in config file

## [0.0.2] - 2019-10-28
### Added
- tests: 004camera 
- core.render: UBO support, OpenGL debug callback
- core.render.context: relative mouse input
- core.render.texture: mipmaps
- core.render.font: dynamic text

### Changed
- badbaby: header guard normalized
- tests.003font: now uses SDF font
- core.common: can load BW targa images
- core.actor: total actor count: hardware_concurency() - 1
- core.render.texture: fixed min/mag select error

## [0.0.1] - 2019-10-20
### Added
- libraries: core.common, core.actor, core.render, core.config
- tests: 000hello, 001render, 002config, 003font
- documentation: README.md, CHANGELOG.md, plan.md, LICENSE.md

[Unreleased]: https://github.com/masscry/badbaby/compare/v0.2.1...develop
[0.2.1]: https://github.com/masscry/badbaby/releases/tag/v0.2.1
[0.2.0]: https://github.com/masscry/badbaby/releases/tag/v0.2.0
[0.1.1]: https://github.com/masscry/badbaby/releases/tag/v0.1.1
[0.1.0]: https://github.com/masscry/badbaby/releases/tag/v0.1.0
[0.0.5]: https://github.com/masscry/badbaby/releases/tag/v0.0.5
[0.0.4]: https://github.com/masscry/badbaby/releases/tag/v0.0.4
[0.0.3]: https://github.com/masscry/badbaby/releases/tag/v0.0.3
[0.0.2]: https://github.com/masscry/badbaby/releases/tag/v0.0.2
[0.0.1]: https://github.com/masscry/badbaby/releases/tag/v0.0.1
