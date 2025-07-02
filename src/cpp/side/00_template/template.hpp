#define __MODULE_INTERFACE__
#include "../../../hpp/globals.hpp"


extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);

void processInput(void * arg);

class ModuleData  
{
public:
    ModuleData() {};
};