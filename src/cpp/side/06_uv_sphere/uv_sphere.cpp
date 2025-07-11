#include "uv_sphere.hpp"

int start_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*)arg;
    AppState * app_state = (AppState*) state->app_state;
    ModuleData * moduleDataPtr = new ModuleData() ;
    state->module_data = moduleDataPtr;


    return 0;
}


int run_module(void *arg) 
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    AppState * app_state = (AppState*) state->app_state;

    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    return 0;
}


int end_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    return 0;
}