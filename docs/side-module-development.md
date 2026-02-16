# Side Module Development

## 1) Create a module folder

Use `src/cpp/side/00_template/` as the starting point.

Typical files to include:

- `<module>.cpp`
- `<module>.hpp`
- `module.json`
- `Makefile`
- optional shaders/textures/assets

## 2) Implement required exports

Each side module must export these C symbols:

```cpp
extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);
```

Lifecycle intent:

- `start_module`: initialize module resources
- `run_module`: per-frame logic/render
- `end_module`: release module resources

## 3) Keep module-local state in `ModuleData`

- Allocate `ModuleData` in `start_module`
- Store pointer in `ModuleState::module_data`
- Read/update it in `run_module`
- Clean up in `end_module`

## 4) Define `module.json`

Example schema:

```json
{
  "title": "My Module",
  "infos": "What this demo does",
  "asset": [
    "my_module.wasm",
    "shader.vs",
    "shader.fs",
    "texture.png"
  ]
}
```

Important: host currently expects `asset[0]` to be the wasm filename.

## 5) Build and deploy

From module directory:

```bash
make all
make deploy
```

If `deploy` is not configured for the module, copy wasm (and assets/module.json) manually into:

`build/webapp_run/modules/<module_folder>/`

## 6) Register module for runtime list

Add its folder name to `resources/json/modules.json`, then ensure the same folder exists under `build/webapp_run/modules/`.

## 7) Optional host ImGui API from side module

If your module needs UI controls, use host-exported C ABI wrappers declared in `src/hpp/gui.hpp` (for example `igBegin`, `igSliderFloat`, `igEnd`).

