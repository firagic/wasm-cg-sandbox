#define __MODULE_INTERFACE__
#define __EMJS_WASM__
#include "../../hpp/globals.hpp"


void ModuleInterface::open_module(void * arg)
{
    // std::cout << "Loading wasm into fs" << std::endl;
    AppState * app_state = (AppState*) arg;
    ModuleState* module_state = (ModuleState*)malloc(sizeof(ModuleState));
    module_state->module_idx = app_state->selected_item;
    module_state->wasm_files_loaded = false;
    module_state->wasm_functions_linked = false;
    app_state->module_state = module_state;
    module_state->app_state = app_state;
    EMJS_WASM::load_wasm_js(app_state->selected_item);
}


void ModuleInterface::link_functions(void * arg) 
{
    AppState * app_state = (AppState*) arg;
    ModuleState* module_state = (ModuleState*) app_state->module_state;
    const char * error;
    // const char * wasm_path = app_state->module_wasms[app_state->selected_item];
    std::cout << app_state->module_wasms->at(app_state->selected_item) << std::endl;
    // module_state->module_handle = dlopen(wasm_path, RTLD_NOW);  
    module_state->module_handle = dlopen(app_state->module_wasms->at(app_state->selected_item).c_str(), RTLD_NOW);  
    if(!module_state->module_handle) {
        std::cout << "Failed to load webassembly " << dlerror() << std::endl;
        return;
    }
    dlerror();  /* Clear any existing error */

    module_state->module_start = (start_module)dlsym(module_state->module_handle, EMJS_WASM::start_module_str);
    module_state->module_run = (run_module)dlsym(module_state->module_handle, EMJS_WASM::run_module_str);
    module_state->module_end = (end_module)dlsym(module_state->module_handle, EMJS_WASM::end_module_str);

    if((error = dlerror())) {
        std::cout << "Failed to load symbol" << error << std::endl;
        dlclose(module_state->module_handle);
        return; 
    }

    module_state->wasm_functions_linked = true;
    return;

}


void ModuleInterface::close_module(void * arg) 
{
    AppState * app_state = (AppState*) arg;
    ModuleState* module_state = (ModuleState*) app_state->module_state;
    EMJS_WASM::unload_wasm_js(module_state->module_idx);
    dlclose(module_state->module_handle);
    module_state->wasm_files_loaded = false;
    module_state->wasm_functions_linked = false;
    module_state->module_start = NULL;
    module_state->module_run = NULL;
    module_state->module_end = NULL;
    module_state->app_state = NULL;
    free(module_state);
    app_state->module_state = NULL;
}


void ModuleInterface::process_module_status(void *arg) 
{
    AppState* app_state = (AppState*)arg;
    ModuleState* module_state = (ModuleState*)app_state->module_state;

    if (!app_state->module_selected) {
        module_state->module_status = MODULE_NOT_SELECTED;

    } else if (module_state == nullptr) {
        module_state->module_status = MODULE_NOT_INITIALIZED;
        
    } else if (!module_state->wasm_files_loaded) {
        module_state->module_status = WAITING_FOR_WASM;
    
    } else if (!module_state->wasm_functions_linked) {
        module_state->module_status = LINKING_FUNCTIONS;

    } else if (module_state->module_idx != app_state->selected_item) {
        module_state->module_status = SWITCHING_MODULE;

    } else {
        module_state->module_status = RUNNING_MODULE;
    }
}


void ModuleInterface::exec_module(void * arg) 
{
    AppState* app_state = (AppState*)arg;
    ModuleState* module_state = (ModuleState*)app_state->module_state;

    process_module_status(app_state);

    switch (module_state->module_status) {
        case MODULE_NOT_SELECTED:
            // Do nothing, break and return
            break;

        case MODULE_NOT_INITIALIZED:
            // Initialize module state
            open_module(app_state);
            break;

        case WAITING_FOR_WASM:
            // Check if WASM files are loaded
            module_state->wasm_files_loaded = EMJS_WASM::check_wasm_loaded();
            break;

        case LINKING_FUNCTIONS:
            // Link functions and start module
            link_functions(app_state);
            module_state->module_start(module_state);
            break;

        case SWITCHING_MODULE:
            // Close current module and prepare to switch
            module_state->module_end(module_state);
            close_module(app_state);
            break;

        case RUNNING_MODULE:
            // Run the module
            module_state->module_run(module_state);
            break;
    }
}

