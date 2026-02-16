# Usage Guide

## Run locally

From repo root:

```bash
source ~/Projects/Graphics/emsdk/emsdk_env.sh
make all
make deploy
make run
```

Open the local server URL shown by Python (typically `http://localhost:8000`).

## Loading demos

- Open the host UI.
- Enable `Examples Window`.
- Select a module from the list.
- The UI shows loading progress while module assets are mounted into Emscripten FS.

## Typical controls

Controls depend on the active module. Common bindings used in multiple modules:

- Camera translation: `W`, `A`, `S`, `D`
- Vertical move: `R`, `F` (in modules that support it)
- Camera yaw/pitch: arrow keys (in modules that support it)

`06_uv_sphere` also includes a config window for sphere parameters and transform values.

## When to rebuild what

- Changed host code (`src/cpp/main`, host JS/HTML, shared headers): rebuild host (`make all && make deploy`).
- Changed side module code/assets: rebuild that module and deploy it.
- Changed host/side ABI contract (module state structures, exported symbols): rebuild both host and affected side modules.

## Troubleshooting

- If module list is empty or loading fails, verify `build/webapp_run/modules.json` and module folders under `build/webapp_run/modules/`.
- If a side module does not load, confirm its wasm filename is the first entry in that module's `module.json` `asset` array.
- If you changed only `build/webapp/index.html`, it will not affect runtime until deployed/copied to `build/webapp_run/index.html`.

