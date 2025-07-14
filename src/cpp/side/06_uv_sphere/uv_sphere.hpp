#define __MODULE_INTERFACE__
#include "../../../hpp/globals.hpp"
#include <unordered_map>

#include "shader.hpp"
#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);

void processInput(void * arg);

class ModuleData  
{
public:
    ModuleData() {};

    Shader * infinity_plane_shader;
    Shader * uv_sphere_shader;

    
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