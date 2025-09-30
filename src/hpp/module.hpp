#ifdef __MODULE_INTERFACE__

#include <stdio.h>

namespace ModuleInterface 
{

    
    
    void exec_module(void *arg);
    
    void open_module(void *arg); 
    
    void link_functions(void *arg); 
    
    void close_module(void *arg);
    
    void process_module_status(void *arg);
    
    
 

}

#endif