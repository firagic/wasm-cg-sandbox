#define __MODULE_INTERFACE__
#include "../../../hpp/globals.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"

extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);

void processInput(void * arg);


class ModuleData  
{
public:
    ModuleData() {};

    Shader * obj_shader;
    Model * obj_model;

    // camera
    Camera * camera;
    float lastX;
    float lastY;
    bool firstMouse;

    // timing
    float deltaTime;
    float lastFrame;
    
    GLFWwindow * window;



};