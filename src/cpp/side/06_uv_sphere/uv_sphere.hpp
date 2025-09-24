#define __MODULE_INTERFACE__
#define __GUI_EXTERN__
#define GLM_ENABLE_EXPERIMENTAL
#include "../../../hpp/globals.hpp"
#include <unordered_map>

#include "shader.hpp"
#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>       // angleAxis, quat_cast, toMat4

extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);

void render_module_gui(void* arg); 
void process_input(void * arg);

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

    int long_zone_count;
    int lat_zone_count;
    
    GLFWwindow * window;

    glm::vec3 uv_sphere_pos;
    glm::vec3 uv_sphere_angle;
    glm::vec3 uv_sphere_last_angle;
    glm::quat uv_sphere_quaternion;

    glm::ivec2 settings_pos;
    glm::ivec2 settings_size;

    glm::vec3 up_vector;
    glm::vec3 right_vector;
    glm::vec3 front_vector; 

    float uv_sphere_scale;

    bool init_done = false;

};