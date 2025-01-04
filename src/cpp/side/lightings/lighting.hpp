#define __MODULE_INTERFACE__
#include "../../../hpp/globals.hpp"


namespace ExampleLighting 
{
    extern "C" int init_module(void *arg);
    
    extern "C" int run_module(void *arg);
    
    extern "C" int clear_module(void *arg);
}
