# Changelog

All notable changes to this project are documented in this file.

The project does not use tagged releases yet, so entries are organized by date and derived from commit history.

## [2026-02-18]

### Changed

- Reworked host UI into a two-pane home layout: `Examples` sidebar plus separate `Landing` window.

## [2026-02-17]

### Added

- New `08_gi_raymarching_sdf` side module with a fullscreen SDF raymarching shader and keyboard camera controls.
- `08_gi_raymarching_sdf` registration in module manifests (`resources/json/modules.json`, `build/webapp/modules.json`, and `build/webapp_run/modules.json`).
- Additional primitives in `07_gi_raytracer` (`box`, `cylinder`, and `torus`) with animated transforms in the fullscreen raytracer scene.
- Animated probe sphere traversal in `07_gi_raytracer`, moving across scene objects on a looping path.

### Changed

- Refactored `07_gi_raytracer` runtime shader setup to use the shared `Shader` helper instead of ad-hoc compile/link helpers and cached uniform locations.
- Updated `08_gi_raymarching_sdf` SDF composition to smooth-union the probe sphere with scene geometry and blend material albedo in transition bands.
- Updated `docs/project-structure.md` with `07_gi_raytracer` and `08_gi_raymarching_sdf` module listings.
- Regenerated runtime outputs (`build/webapp/` and `build/webapp_run/`) to include current module manifests and shader/module assets.
- Updated keyboard camera rotation in `07_gi_raytracer` and `08_gi_raymarching_sdf` to be time-based (`deltaTime`) instead of frame-based, with tuned turn sensitivity for a more consistent feel.

### Fixed

- Adjusted module GL-state reset behavior in `src/cpp/main/module.cpp` by removing the forced `GL_PRIMITIVE_RESTART_FIXED_INDEX` disable.
- Prevented first-frame camera timing spikes in `07_gi_raytracer` and `08_gi_raymarching_sdf` by initializing `last_frame` from `glfwGetTime()` during module startup.

## [2026-02-16]

### Added

- New `07_gi_raytracer` side module with a fullscreen shader raytracer, animated scene, and keyboard camera controls.
- `07_gi_raytracer` registration in the runtime module manifest (`resources/json/modules.json`).
- New project documentation set under `docs/` (overview, structure, build/deploy, runtime architecture, usage, and side-module development guides).
- Additional bundled fonts for UI rendering (`pragmasevka-regular.ttf`, `IosevkaTerm-Regular.ttf`, `JetBrainsMono-Regular.ttf`).

### Changed

- Refactored JS bridge/runtime helpers in `src/hpp/emjs.hpp` to use a shared runtime state object (`Module.__cgRuntime`) for module list, loading flags, and progress.
- Moved module manifest loading and asset load/unload flow from `resources/htmls/index.html` into `EM_JS` bindings.
- Updated app startup/module init flow in `src/cpp/main/app.cpp` to initialize runtime state, resize canvas, and read module manifests before module setup.
- Updated host GUI font loading and labels in `src/cpp/main/gui.cpp`, including fallback to default font if custom font loading fails.
- Updated root deploy rule in `Makefile` to copy `app.data` alongside `app.wasm` and `app.js`.
- Regenerated runtime build outputs in `build/webapp/` and `build/webapp_run/` to match current sources and module list.

### Fixed

- Prevented missing deployed data package by including `app.data` in deploy output.
- Improved runtime safety around module loading/unloading with index checks, default fallbacks, and guarded state access.

## [2025-10-14]

### Changed

- Minor updates to makefiles and switch-case logic.

### Fixed

- Corrected initial value handling in the UV sphere module.

## [2025-10-06]

### Changed

- Reworked the module interface structure and added a `loaded` flag to main state.

## [2025-10-02]

### Changed

- Updated the HTML shell/runtime page.

## [2025-10-01]

### Added

- Module loaded percentage indicator.

### Changed

- Updated the main `Makefile`.

## [2025-09-30]

### Added

- Per-module loading indicator support.

### Changed

- Moved module state definition to a global header.

## [2025-09-24]

### Added

- Additional controls and transformation options.

### Changed

- Modified deploy script.
- Updated default panel position.
- Fixed makefile issues.

## [2025-08-22]

### Added

- GUI support in side modules.

### Fixed

- UV sphere shader fixes.

## [2025-07-14]

### Changed

- Updated the `06_uv_sphere` example.
- Rearranged build output directory structure.

## [2025-07-11]

### Added

- Infinity plane module.
- New `uv_sphere` module.

### Changed

- Updated side-module template.

## [2025-07-02]

### Added

- Ordering numbers for side modules to simplify module management.
- Endless grid example module.

## [2025-01-19]

### Added

- Side-module template and boilerplate.

## [2025-01-17]

### Changed

- Main GUI updates.

## [2025-01-14]

### Added

- Model loading example module.

### Changed

- General cleanup and fixes.
- Removed debug prints.

## [2025-01-10]

### Changed

- Lightings example cleanup.

## [2025-01-09]

### Added

- App state reference inside module state for module access.
- WASD camera control in the lightings example.

## [2025-01-08]

### Added

- Lightings example module.

## [2025-01-04]

### Added

- Initial project scaffold.

### Changed

- Naming updates.
- README updates.
