#define __MODULE_INTERFACE__
#include "../../../hpp/globals.hpp"

#include <unordered_map>
#include <string>
#include <vector>

#include "camera.hpp"

extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);
void process_input(void *arg);

class ModuleData
{
public:
    ModuleData() {}

    GLuint shader_program = 0;
    GLuint quad_vao = 0;
    GLuint quad_vbo = 0;

    GLint u_resolution = -1;
    GLint u_time = -1;
    GLint u_cam_pos = -1;
    GLint u_cam_front = -1;
    GLint u_cam_up = -1;
    GLint u_fov = -1;

    Camera *camera = NULL;
    float delta_time = 0.0f;
    float last_frame = 0.0f;
    GLFWwindow *window = NULL;
};
