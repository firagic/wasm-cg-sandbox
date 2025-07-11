#define __MODULE_INTERFACE__
#include "../../../hpp/globals.hpp"

#include "shader.hpp"
#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);

void processInput(void * arg);

struct CameraMatrices {
    glm::mat4 view;
    glm::mat4 projection;
}; 

class ModuleData  
{
public:
    ModuleData() {};

    Shader * infinity_plane_shader;
    unsigned int infinity_plane_VAO;
    unsigned int camera_UBO;
    
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