#include "gi_raytracer.hpp"

#include <iostream>

int start_module(void *arg)
{
    ModuleInterface::ModuleState *state = (ModuleInterface::ModuleState *)arg;
    AppState *app_state = (AppState *)state->app_state;
    ModuleData *module_data = new ModuleData();
    state->module_data = module_data;

    module_data->shader = new Shader("gi_raytracer.vs", "gi_raytracer.fs");

    module_data->camera = new Camera(glm::vec3(0.0f, 0.9f, 7.5f));
    module_data->window = app_state->g_window;
    module_data->delta_time = 0.0f;
    module_data->last_frame = static_cast<float>(glfwGetTime());

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

    if (!module_data->shader || module_data->shader->ID == 0) {
        std::cerr << "Failed to create shader program for gi_raytracer." << std::endl;
        return 0;
    }

    return 0;
}

int run_module(void *arg)
{
    ModuleInterface::ModuleState *state = (ModuleInterface::ModuleState *)arg;
    AppState *app_state = (AppState *)state->app_state;
    ModuleData *module_data = (ModuleData *)state->module_data;

    if (!module_data || module_data->shader == NULL || module_data->shader->ID == 0) {
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

    module_data->shader->use();
    module_data->shader->setVec2("uResolution", (float)app_state->canvas_width, (float)app_state->canvas_height);
    module_data->shader->setFloat("uTime", current_frame);
    module_data->shader->setVec3("uCamPos", module_data->camera->Position);
    module_data->shader->setVec3("uCamFront", module_data->camera->Front);
    module_data->shader->setVec3("uCamUp", module_data->camera->Up);
    module_data->shader->setFloat("uFov", module_data->camera->Zoom);

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
    if (module_data->shader != NULL) {
        if (module_data->shader->ID != 0) {
            glDeleteProgram(module_data->shader->ID);
        }
        delete module_data->shader;
        module_data->shader = NULL;
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
