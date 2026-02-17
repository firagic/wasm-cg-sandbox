#define __MODULE_INTERFACE__
#include "../../../hpp/globals.hpp"

#include <string>
#include <unordered_map>
#include <vector>

#include "shader.hpp"
#include "camera.hpp"

extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);
void process_input(void *arg);

class ModuleData
{
public:
    ModuleData() {}

    Shader *shader = NULL;
    GLuint quad_vao = 0;
    GLuint quad_vbo = 0;

    Camera *camera = NULL;
    float delta_time = 0.0f;
    float last_frame = 0.0f;
    GLFWwindow *window = NULL;
};
