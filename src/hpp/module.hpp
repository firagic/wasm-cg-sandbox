#ifdef __MODULE_INTERFACE__

#include <stdio.h>

namespace ModuleInterface 
{

    typedef int (*start_module)(void*);
    
    typedef int (*run_module)(void*);
    
    typedef int (*end_module)(void*);


    void exec_module(void *arg);

    void open_module(void *arg); 
    
    void link_functions(void *arg); 

    void close_module(void *arg);

    void process_module_status(void *arg);


    enum ModuleStatus {
        MODULE_NOT_SELECTED,
        MODULE_NOT_INITIALIZED,
        WAITING_FOR_WASM,
        LINKING_FUNCTIONS,
        SWITCHING_MODULE,
        RUNNING_MODULE
    };

    typedef struct {
        int module_idx; 
        unsigned int VBO;
        unsigned int VAO; 
        unsigned int shaderProgram;

        bool wasm_files_loaded;
        bool wasm_functions_linked;
        void * module_handle;

        start_module module_start;
        run_module module_run;
        end_module module_end;

        ModuleStatus module_status;

    } ModuleState;


}

#endif