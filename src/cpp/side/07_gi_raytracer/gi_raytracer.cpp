#include "gi_raytracer.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

static std::string read_text_file(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint compile_shader(GLenum type, const char *source, const char *label)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[1024];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        std::cerr << "Shader compile error (" << label << "): " << info_log << std::endl;
    }
    return shader;
}

static GLuint create_program_from_files(const char *vs_path, const char *fs_path)
{
    std::string vs_code = read_text_file(vs_path);
    std::string fs_code = read_text_file(fs_path);
    if (vs_code.empty() || fs_code.empty()) {
        return 0;
    }

    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_code.c_str(), vs_path);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_code.c_str(), fs_path);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[1024];
        glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
        std::cerr << "Program link error: " << info_log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int start_module(void *arg)
{
    ModuleInterface::ModuleState *state = (ModuleInterface::ModuleState *)arg;
    AppState *app_state = (AppState *)state->app_state;
    ModuleData *module_data = new ModuleData();
    state->module_data = module_data;

    module_data->shader_program = create_program_from_files("gi_raytracer.vs", "gi_raytracer.fs");

    module_data->camera = new Camera(glm::vec3(0.0f, 0.9f, 7.5f));
    module_data->window = app_state->g_window;
    module_data->delta_time = 0.0f;
    module_data->last_frame = 0.0f;

    static const float quad_vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    glGenVertexArrays(1, &module_data->quad_vao);
    glGenBuffers(1, &module_data->quad_vbo);
    glBindVertexArray(module_data->quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, module_data->quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    module_data->u_resolution = glGetUniformLocation(module_data->shader_program, "uResolution");
    module_data->u_time = glGetUniformLocation(module_data->shader_program, "uTime");
    module_data->u_cam_pos = glGetUniformLocation(module_data->shader_program, "uCamPos");
    module_data->u_cam_front = glGetUniformLocation(module_data->shader_program, "uCamFront");
    module_data->u_cam_up = glGetUniformLocation(module_data->shader_program, "uCamUp");
    module_data->u_fov = glGetUniformLocation(module_data->shader_program, "uFov");

    return 0;
}

int run_module(void *arg)
{
    ModuleInterface::ModuleState *state = (ModuleInterface::ModuleState *)arg;
    AppState *app_state = (AppState *)state->app_state;
    ModuleData *module_data = (ModuleData *)state->module_data;

    if (!module_data || module_data->shader_program == 0) {
        return 0;
    }

    float current_frame = static_cast<float>(glfwGetTime());
    module_data->delta_time = current_frame - module_data->last_frame;
    module_data->last_frame = current_frame;
    process_input(arg);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glClearColor(0.02f, 0.02f, 0.03f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(module_data->shader_program);
    glUniform2f(module_data->u_resolution, (float)app_state->canvas_width, (float)app_state->canvas_height);
    glUniform1f(module_data->u_time, current_frame);
    glUniform3f(module_data->u_cam_pos,
        module_data->camera->Position.x,
        module_data->camera->Position.y,
        module_data->camera->Position.z);
    glUniform3f(module_data->u_cam_front,
        module_data->camera->Front.x,
        module_data->camera->Front.y,
        module_data->camera->Front.z);
    glUniform3f(module_data->u_cam_up,
        module_data->camera->Up.x,
        module_data->camera->Up.y,
        module_data->camera->Up.z);
    glUniform1f(module_data->u_fov, module_data->camera->Zoom);

    glBindVertexArray(module_data->quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    return 0;
}

int end_module(void *arg)
{
    ModuleInterface::ModuleState *state = (ModuleInterface::ModuleState *)arg;
    ModuleData *module_data = (ModuleData *)state->module_data;
    if (!module_data) {
        return 0;
    }

    if (module_data->quad_vbo != 0) {
        glDeleteBuffers(1, &module_data->quad_vbo);
    }
    if (module_data->quad_vao != 0) {
        glDeleteVertexArrays(1, &module_data->quad_vao);
    }
    if (module_data->shader_program != 0) {
        glDeleteProgram(module_data->shader_program);
    }
    if (module_data->camera != NULL) {
        delete module_data->camera;
        module_data->camera = NULL;
    }

    delete module_data;
    state->module_data = NULL;
    return 0;
}

void process_input(void *arg)
{
    static const std::unordered_map<int, Camera_Movement> key_movement_map = {
        {GLFW_KEY_R, Camera_Movement::UP},
        {GLFW_KEY_F, Camera_Movement::DOWN},
        {GLFW_KEY_W, Camera_Movement::FORWARD},
        {GLFW_KEY_S, Camera_Movement::BACKWARD},
        {GLFW_KEY_A, Camera_Movement::LEFT},
        {GLFW_KEY_D, Camera_Movement::RIGHT},
        {GLFW_KEY_UP, Camera_Movement::PITCH_UP},
        {GLFW_KEY_DOWN, Camera_Movement::PITCH_DOWN},
        {GLFW_KEY_LEFT, Camera_Movement::YAW_LEFT},
        {GLFW_KEY_RIGHT, Camera_Movement::YAW_RIGHT}
    };

    ModuleInterface::ModuleState *state = (ModuleInterface::ModuleState *)arg;
    ModuleData *module_data = (ModuleData *)state->module_data;
    if (!module_data || module_data->camera == NULL || module_data->window == NULL) {
        return;
    }

    for (const auto &[key, movement] : key_movement_map) {
        if (glfwGetKey(module_data->window, key) == GLFW_PRESS) {
            module_data->camera->ProcessKeyboard(movement, module_data->delta_time);
        }
    }
}
