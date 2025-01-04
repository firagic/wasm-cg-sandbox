#ifdef __EMJS__

namespace EMJS 
{
    // Function used by c++ to get the width of the html canvas
    EM_JS(int, canvas_get_width, (), {
        return Module.canvas.width;
    });

    // Function used by c++ to get the height of the html canvas
    EM_JS(int, canvas_get_height, (), {
        return Module.canvas.height;
    });

    // Wrapper function that calls resize canvas 
    EM_JS(void, resize_canvas, (), {
        js_resizeCanvas();
    });
    // JS function used by c++ to read the list of all side modules
    EM_JS(bool, check_read_flag, (), {
        return moduleListReadFlag; ; 
    });

    // JS function used by c++ to get module list json as string 
    EM_JS(const char*, get_modules_json_data, (), {
        return allocate(intArrayFromString(moduleList), ALLOC_NORMAL); ; 
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
        loadWasmToFS(selected_module_index); // Call the JS function
    });
     
    // Wrapper function to check if wasm is loaded into the emscripten file system
    EM_JS(bool, check_wasm_loaded, (), {
        return wasmLoadedIntoFS;
    });

    // Wrapper function unload the wasm module
    // EM_JS(void, unload_wasm_js, (const char* filePathStr), {
    //     unlinkWasmFromFS(filePathStr);
    // });
    EM_JS(void, unload_wasm_js, (int selected_module_index), {
        unlinkWasmFromFS(selected_module_index);
    });

}

#endif
