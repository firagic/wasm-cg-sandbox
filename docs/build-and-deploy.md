# Build and Deploy

## Prerequisites

- Emscripten SDK installed locally.
- `imgui` repository present at `../imgui` (required by root `Makefile`).
- Python 3 for local HTTP serving (`python3 -m http.server`).

## Environment setup

Recommended once per terminal session:

```bash
source ~/Projects/Graphics/emsdk/emsdk_env.sh
emcc -v
```

The project includes helper scripts under `resources/scripts/` and each side module folder, but sourcing `emsdk_env.sh` once manually is faster for repeated builds.

## Main module build/deploy

From repo root:

```bash
make all
make deploy
```

What this does:

- Compiles host app to `build/webapp/app.{js,wasm,data}`
- Copies `resources/htmls/index.html` and `resources/json/modules.json` into `build/webapp/`
- Deploy target copies host files into `build/webapp_run/`

`make run` serves `build/webapp_run`.

## Side module build/deploy

Each side module is built from its own folder.

Example:

```bash
cd src/cpp/side/06_uv_sphere
make all
make deploy
```

Outputs are written to `build/modules/...` and deploy copies wasm into `build/webapp_run/modules/...`.

## Full rebuild workflow

Current practical workflow:

```bash
# 1) rebuild host
make all && make deploy

# 2) rebuild side modules you changed
(cd src/cpp/side/05_endless_grid && make all && make deploy)
(cd src/cpp/side/06_uv_sphere && make all && make deploy)

# 3) run
make run
```

## Current caveats

- Root `make all` does not build side modules automatically.
- Side-module deploy support is inconsistent:
  - `05_endless_grid` and `06_uv_sphere` define `DEPLOY_PATH`.
  - `01_hello_triangle`, `03_lightings`, and `04_process_model` have a `deploy` target that references `DEPLOY_PATH` but do not define it.
- `resources/json/modules.json` controls what module folders are loaded at runtime, so deploy folder names must match it.

## Build artifacts summary

- Host build output: `build/webapp/`
- Runtime served folder: `build/webapp_run/`
- Side intermediate output: `build/modules/`
- Side runtime folder: `build/webapp_run/modules/<module_name>/`

