#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <dlfcn.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#endif

#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "json.hpp"
using json = nlohmann::json;

#include "gui.hpp"
#include "app.hpp"
#include "emjs.hpp"
#include "module.hpp"

enum ModuleStatus {
    MODULE_NOT_SELECTED,
    MODULE_NOT_INITIALIZED,
    WAITING_FOR_WASM,
    LINKING_FUNCTIONS,
    SWITCHING_MODULE,
    RUNNING_MODULE
};

typedef int (*start_module)(void*);

typedef int (*run_module)(void*);

typedef int (*end_module)(void*);

typedef struct {
    int module_idx; 
    unsigned int VBO;
    unsigned int VAO; 
    unsigned int shaderProgram;

    bool wasm_files_loaded;
    bool wasm_functions_linked;
    void * module_handle;
    void * module_data;

    start_module module_start;
    run_module module_run;
    end_module module_end;

    ModuleStatus module_status;

    void * app_state;

} ModuleState;

typedef struct {
    GLFWwindow *g_window;
    // ImVec4 clear_color;
    bool show_demo_window;
    bool show_another_window;
    int g_width;
    int g_height;
    int canvas_width;
    int canvas_height;
    int display_width;
    int display_height;
    float data;
 
    void * module_state;

    int selected_item;
    bool module_selected;
    void * module_list;
    // char ** module_names;
    // char ** module_paths;
    // char ** module_descs;
    // char ** module_wasms;

    std::vector<std::string> * module_wasms;
    std::vector<std::string> * module_names;

    // std::vector<void*> module_handles;

} AppState;

