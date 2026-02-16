# Runtime Architecture

## High-level components

1. Browser shell and JS bootstrap (`resources/htmls/index.html`)
2. Host wasm application (`src/cpp/main/*`)
3. Dynamically loaded side wasm modules (`src/cpp/side/*`)
4. ImGui integration and C ABI bridge (`src/cpp/main/gui.cpp`)

## Startup flow

1. Browser loads `index.html` and Emscripten-generated `app.js`.
2. JS fetches `modules.json` and each `/modules/<folder>/module.json`.
3. C++ host starts (`main.cpp` -> `App::run`).
4. Host initializes GLFW/OpenGL/ImGui and enters frame loop.

## Frame loop responsibilities

In each frame (`App::loop`):

- Sync canvas/window size
- Poll events
- Set viewport and clear frame
- Begin ImGui frame
- Execute module lifecycle state machine
- Draw host UI and render ImGui

## Side-module lifecycle

The host expects every side module to export:

- `start_module(void*)`
- `run_module(void*)`
- `end_module(void*)`

The host module manager (`module.cpp`) does:

- Load module assets into Emscripten FS (via JS bridge)
- `dlopen` side wasm
- `dlsym` exported lifecycle functions
- Call `start_module` once
- Call `run_module` every frame while active
- Call `end_module` when switching away

## Runtime data contract

- `AppState` stores global app/runtime state (window, dimensions, selected module, etc.).
- `ModuleState` stores module-level handles/function pointers and opaque `module_data`.
- Side modules allocate their own `ModuleData` and store it in `ModuleState::module_data`.

## Module discovery contract

- Runtime module list: `resources/json/modules.json`
- For each module folder listed there, runtime expects:
  - `/modules/<folder>/module.json`
  - assets listed in `module.json`

Important: host currently assumes the first asset entry (`asset[0]`) is the side wasm filename.

## ImGui bridge for side modules

Host exports a C ABI wrapper surface (`igBegin`, `igSliderFloat`, etc.) so side modules can render UI without linking full ImGui independently.

