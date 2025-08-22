#include "uv_sphere.hpp"

int start_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*)arg;
    AppState * app_state = (AppState*) state->app_state;
    ModuleData * moduleDataPtr = new ModuleData() ;
    state->module_data = moduleDataPtr;

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    moduleDataPtr->camera = new Camera (glm::vec3(0.0f, 1.0f, 0.0f));
    moduleDataPtr->lastX = app_state->display_width / 2.0f;
    moduleDataPtr->lastY = app_state->display_height / 2.0f;
    moduleDataPtr->firstMouse = true;
    moduleDataPtr->deltaTime = 0.0f;
    moduleDataPtr->lastFrame = 0.0f;
    moduleDataPtr->window = app_state->g_window;
    moduleDataPtr->lat_zone_count = 8;
    moduleDataPtr->long_zone_count = 12;

    moduleDataPtr->infinity_plane_shader = new Shader("infinity_plane.vs", "infinity_plane.fs"); 
    moduleDataPtr->uv_sphere_shader = new Shader("uv_sphere.vs", "uv_sphere.fs"); 


    return 0;
}


int run_module(void *arg) 
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    AppState * app_state = (AppState*) state->app_state;

    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    // -----------
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    moduleDataPtr->deltaTime = currentFrame - moduleDataPtr->lastFrame;
    moduleDataPtr->lastFrame = currentFrame;
    
    // input
    // -----
    process_input(state);
    
    // render
    // ------
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up identity view and projection
    glm::mat4 projection = glm::perspective(glm::radians(moduleDataPtr->camera->Zoom), (float)app_state->canvas_width/ (float)app_state->canvas_height, 0.1f, 100.0f);
    glm::mat4 view = moduleDataPtr->camera->GetViewMatrix();
    
    moduleDataPtr->infinity_plane_shader->use();
    moduleDataPtr->infinity_plane_shader->setMat4("projection", projection);
    moduleDataPtr->infinity_plane_shader->setMat4("view", view); 
    moduleDataPtr->infinity_plane_shader->setVec3("camera_world_pos", moduleDataPtr->camera->Position);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glBindVertexArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    moduleDataPtr->uv_sphere_shader->use();
    moduleDataPtr->uv_sphere_shader->setMat4("projection", projection);
    moduleDataPtr->uv_sphere_shader->setMat4("view", view); 
    moduleDataPtr->uv_sphere_shader->setFloat("sectors", moduleDataPtr->long_zone_count);
    moduleDataPtr->uv_sphere_shader->setFloat("stacks", moduleDataPtr->lat_zone_count);


    glDrawArrays(GL_TRIANGLES, 0, 24 * 18 * 6);
    glDisable(GL_BLEND);

    // gui
    render_module_gui(state);

    return 0;
}


int end_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    free(moduleDataPtr);

    return 0;
}


void process_input(void* arg) 
{
    static const std::unordered_map<int, Camera_Movement> keyMovementMap = {
        {GLFW_KEY_R     , Camera_Movement::UP},
        {GLFW_KEY_F     , Camera_Movement::DOWN},
        {GLFW_KEY_W     , Camera_Movement::FORWARD},
        {GLFW_KEY_S     , Camera_Movement::BACKWARD},
        {GLFW_KEY_A     , Camera_Movement::LEFT},
        {GLFW_KEY_D     , Camera_Movement::RIGHT},
        {GLFW_KEY_UP    , Camera_Movement::PITCH_UP},
        {GLFW_KEY_DOWN  , Camera_Movement::PITCH_DOWN},
        {GLFW_KEY_LEFT  , Camera_Movement::YAW_LEFT},
        {GLFW_KEY_RIGHT , Camera_Movement::YAW_RIGHT}
    };
    
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    for (const auto& [key, movement] : keyMovementMap) {
        if (glfwGetKey(moduleDataPtr->window, key) == GLFW_PRESS) {
            moduleDataPtr->camera->ProcessKeyboard(movement, moduleDataPtr->deltaTime);
            // std::cout << "Key " << key << " pressed" << std::endl;
        }
    }
}

void render_module_gui(void* arg) {

    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    igSetNextWindowPos(((AppState*) state->app_state)->canvas_width - 300, 0);
    igSetNextWindowSize(300, 200);
    
    igBegin("Config");
    igSliderInt("Latitude", &moduleDataPtr->lat_zone_count, 8, 16);
    igSliderInt("Longtitude", &moduleDataPtr->long_zone_count, 12, 24);
    igEnd();
};

