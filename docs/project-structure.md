# Project Structure

## Root layout

```text
wasm-cg-sandbox/
├── Makefile
├── README.md
├── resources/
│   ├── htmls/
│   ├── json/
│   ├── fonts/
│   └── scripts/
├── src/
│   ├── hpp/
│   └── cpp/
│       ├── main/
│       └── side/
├── build/
│   ├── webapp/
│   ├── webapp_run/
│   └── modules/
└── docs/
```

## Source of truth

- Host runtime (main module): `src/cpp/main/` + `src/hpp/`
- Side modules (dynamic wasm demos): `src/cpp/side/*`
- Browser shell and runtime JS bootstrap: `resources/htmls/index.html`
- Module menu manifest used at runtime: `resources/json/modules.json`

## Important folders

### `src/cpp/main/`

Main application compiled as the `MAIN_MODULE`.

- `main.cpp`: process entry
- `app.cpp`: app init + frame loop
- `module.cpp`: side-module loading/linking/state machine
- `gui.cpp`: host ImGui and C ABI wrappers exposed to side modules

### `src/cpp/side/`

Side-module demos compiled as `SIDE_MODULE` wasm files.

- `00_template`: starter scaffold for new modules
- `01_hello_triangle`: basic triangle demo
- `03_lightings`: lighting/material demo
- `04_process_model`: OBJ model loading demo
- `05_endless_grid`: infinite grid demo
- `06_uv_sphere`: procedural UV sphere demo

Note: each side module has its own `Makefile`, `compile.sh`, and (usually) `module.json`.

### `resources/`

- `resources/htmls/index.html`: app shell + JS glue used by Emscripten runtime
- `resources/json/modules.json`: list of runtime module folders (shown in menu)
- `resources/fonts/`: fonts preloaded into Emscripten FS for ImGui
- `resources/scripts/compile.sh`: helper script for host build

### `build/`

Generated artifacts.

- `build/webapp/`: output from `make all` for host module
- `build/webapp_run/`: runtime serving directory (`make run` serves this folder)
- `build/modules/`: output location for side module builds

## Third-party/vendor code in tree

Several side modules include vendored code such as `glm`, `lodepng`, and `tinyobjloader` inside their module folders.

