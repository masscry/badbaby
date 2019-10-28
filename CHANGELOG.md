# Changelog

All notable changes to this project will be documented in this file

## [Unreleased]

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

[Unreleased]: https://github.com/masscry/badbaby/compare/v0.0.2...HEAD
[0.0.2]: https://github.com/masscry/badbaby/releases/tag/v0.0.2
[0.0.1]: https://github.com/masscry/badbaby/releases/tag/v0.0.1
