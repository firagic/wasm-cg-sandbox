#ifdef __EMJS__

namespace EMJS 
{
    EM_JS(void, init_runtime_state, (), {
        if (!Module.__cgRuntime) {
            Module.__cgRuntime = {
                wasmLoadedIntoFS: false,
                resourceCount: 0,
                percentageLoaded: 0,
                moduleList: "[]",
                moduleListJson: [],
                moduleListReadFlag: false,
                moduleListLoading: false,
                resizeBound: false
            };
        }
    });

    // Function used by c++ to get the width of the html canvas
    EM_JS(int, canvas_get_width, (), {
        return Module.canvas.width;
    });

    // Function used by c++ to get the height of the html canvas
    EM_JS(int, canvas_get_height, (), {
        return Module.canvas.height;
    });

    // Wrapper function that sets canvas size and binds resize handler once
    EM_JS(void, resize_canvas, (), {
        if (!Module.__cgRuntime) {
            Module.__cgRuntime = {
                wasmLoadedIntoFS: false,
                resourceCount: 0,
                percentageLoaded: 0,
                moduleList: "[]",
                moduleListJson: [],
                moduleListReadFlag: false,
                moduleListLoading: false,
                resizeBound: false
            };
        }

        if (!Module.__cgRuntime.resizeBound) {
            window.addEventListener('resize', function () {
                if (Module.canvas) {
                    Module.canvas.width = window.innerWidth;
                    Module.canvas.height = window.innerHeight;
                }
            }, false);
            Module.__cgRuntime.resizeBound = true;
        }

        if (Module.canvas) {
            Module.canvas.width = window.innerWidth;
            Module.canvas.height = window.innerHeight;
        }
    });

    // Read modules.json + per-module module.json and cache in runtime state
    EM_JS(void, read_modules_manifest, (), {
        if (!Module.__cgRuntime) {
            Module.__cgRuntime = {
                wasmLoadedIntoFS: false,
                resourceCount: 0,
                percentageLoaded: 0,
                moduleList: "[]",
                moduleListJson: [],
                moduleListReadFlag: false,
                moduleListLoading: false,
                resizeBound: false
            };
        }

        if (Module.__cgRuntime.moduleListLoading || Module.__cgRuntime.moduleListReadFlag) {
            return;
        }

        Module.__cgRuntime.moduleListLoading = true;
        try {
            var response = new XMLHttpRequest();
            response.open("GET", "modules.json", false);
            response.send(null);
            if (response.status !== 200) {
                throw new Error("Network response was not ok");
            }

            var folderPaths = JSON.parse(response.responseText);
            var modules = [];

            for (var i = 0; i < folderPaths.length; i++) {
                var folderPath = folderPaths[i];
                try {
                    var moduleResponse = new XMLHttpRequest();
                    moduleResponse.open("GET", "/modules/" + folderPath + "/module.json", false);
                    moduleResponse.send(null);
                    if (moduleResponse.status !== 200) {
                        throw new Error("Failed to load module.json from " + folderPath);
                    }

                    modules.push({
                        name: folderPath,
                        data: JSON.parse(moduleResponse.responseText)
                    });
                } catch (moduleError) {
                    console.error("Error loading module.json for " + folderPath + ":", moduleError);
                    modules.push({
                        name: folderPath,
                        data: { asset: [] }
                    });
                }
            }

            Module.__cgRuntime.moduleListJson = modules;
            Module.__cgRuntime.moduleList = JSON.stringify(modules, null, 2);
        } catch (error) {
            console.error("Failed to load main JSON or process data:", error);
            Module.__cgRuntime.moduleListJson = [];
            Module.__cgRuntime.moduleList = "[]";
        } finally {
            Module.__cgRuntime.moduleListReadFlag = true;
            Module.__cgRuntime.moduleListLoading = false;
        }
    });

    // JS function used by c++ to read the list of all side modules
    EM_JS(bool, check_read_flag, (), {
        if (!Module.__cgRuntime) {
            return false;
        }
        return !!Module.__cgRuntime.moduleListReadFlag;
    });

    // JS function used by c++ to get module list json as string 
    EM_JS(const char*, get_modules_json_data, (), {
        var payload = "[]";
        if (Module.__cgRuntime && typeof Module.__cgRuntime.moduleList === "string") {
            payload = Module.__cgRuntime.moduleList;
        }
        return allocate(intArrayFromString(payload), ALLOC_NORMAL);
    });
}
#endif

#ifdef __EMJS_WASM__

namespace EMJS_WASM
{

    const char * module_wasm_file = "module.wasm";

    const char * start_module_str = "start_module";
    
    const char * run_module_str = "run_module";
    
    const char * end_module_str = "end_module";


    // Wrapper function that calls loadWasmToFS
    // EM_JS(void, load_wasm_js, (const char* urlStr, const char* filePathStr), {
    //     loadWasmToFS(urlStr, filePathStr); // Call the JS function
    // });

    EM_JS(void, load_wasm_js, (int selected_module_index), {
        if (!Module.__cgRuntime) {
            Module.__cgRuntime = {
                wasmLoadedIntoFS: false,
                resourceCount: 0,
                percentageLoaded: 0,
                moduleList: "[]",
                moduleListJson: [],
                moduleListReadFlag: false,
                moduleListLoading: false,
                resizeBound: false
            };
        }

        Module.__cgRuntime.wasmLoadedIntoFS = false;
        Module.__cgRuntime.percentageLoaded = 0;

        var modules = Module.__cgRuntime.moduleListJson || [];
        if (selected_module_index < 0 || selected_module_index >= modules.length) {
            console.error("Invalid module index:", selected_module_index);
            return;
        }

        var moduleEntry = modules[selected_module_index];
        var assets = [];
        if (moduleEntry && moduleEntry.data && Array.isArray(moduleEntry.data.asset)) {
            assets = moduleEntry.data.asset;
        }
        Module.__cgRuntime.resourceCount = assets.length;
        var baseUrl = "/modules/" + moduleEntry.name;

        (async function () {
            try {
                if (assets.length === 0) {
                    Module.__cgRuntime.percentageLoaded = 100;
                    Module.__cgRuntime.wasmLoadedIntoFS = true;
                    return;
                }

                for (var i = 0; i < assets.length; i++) {
                    var fileStr = assets[i];
                    var response = await fetch(baseUrl + "/" + fileStr);
                    if (!response.ok) {
                        throw new Error("Failed to fetch " + fileStr + " from " + baseUrl);
                    }

                    var buffer = await response.arrayBuffer();
                    var fullPath = "/" + fileStr;
                    if (FS.analyzePath(fullPath).exists) {
                        FS.unlink(fileStr);
                    }
                    FS.createDataFile("/", fileStr, new Uint8Array(buffer), true, true, true);
                    console.log(fileStr + " has been added to the Emscripten file system.");
                    Module.__cgRuntime.percentageLoaded = Math.floor(((i + 1) / assets.length) * 100);
                }

                Module.__cgRuntime.wasmLoadedIntoFS = true;
            } catch (error) {
                console.error("Error loading WASM file:", error);
                Module.__cgRuntime.wasmLoadedIntoFS = false;
            }
        })();
    });
     
    // Wrapper function to check if wasm is loaded into the emscripten file system
    EM_JS(bool, check_wasm_loaded, (), {
        if (!Module.__cgRuntime) {
            return false;
        }
        return !!Module.__cgRuntime.wasmLoadedIntoFS;
    });

    EM_JS(int, get_percent_completed, (), {
        if (!Module.__cgRuntime) {
            return 0;
        }
        var percentageLoaded = Module.__cgRuntime.percentageLoaded;
        return (typeof percentageLoaded === "number") ? percentageLoaded : parseInt(percentageLoaded) || 0;
    });

    // Wrapper function unload the wasm module
    // EM_JS(void, unload_wasm_js, (const char* filePathStr), {
    //     unlinkWasmFromFS(filePathStr);
    // });
    EM_JS(void, unload_wasm_js, (int selected_module_index), {
        if (!Module.__cgRuntime || !Module.__cgRuntime.moduleListJson) {
            return;
        }

        var modules = Module.__cgRuntime.moduleListJson;
        if (selected_module_index < 0 || selected_module_index >= modules.length) {
            return;
        }

        try {
            var moduleEntry = modules[selected_module_index];
            var assets = [];
            if (moduleEntry && moduleEntry.data && Array.isArray(moduleEntry.data.asset)) {
                assets = moduleEntry.data.asset;
            }

            for (var i = 0; i < assets.length; i++) {
                var fileStr = assets[i];
                var fullPath = "/" + fileStr;
                if (FS.analyzePath(fullPath).exists) {
                    FS.unlink(fileStr);
                    console.log("File '" + fileStr + "' '" + selected_module_index + "' has been successfully deleted.");
                }
            }
        } catch (error) {
            console.error("Error deleting file '" + selected_module_index + "':", error);
        }

        Module.__cgRuntime.wasmLoadedIntoFS = false;
        Module.__cgRuntime.percentageLoaded = 0;
    });

}

#endif
