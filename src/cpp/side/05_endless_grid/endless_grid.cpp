#include "endless_grid.hpp"

int start_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*)arg;
    AppState * app_state = (AppState*) state->app_state;
    ModuleData * moduleDataPtr = new ModuleData();
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

    moduleDataPtr->infinity_plane_shader = new Shader("infinity_plane.vs", "infinity_plane.fs"); 
    
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
    processInput(state);
    
    // render
    // ------
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    moduleDataPtr->infinity_plane_shader->use();

    glm::mat4 projection = glm::perspective(glm::radians(moduleDataPtr->camera->Zoom), (float)app_state->canvas_width/ (float)app_state->canvas_height, 0.1f, 100.0f);
    glm::mat4 view = moduleDataPtr->camera->GetViewMatrix();
    
    // Set up identity view and projection
    moduleDataPtr->infinity_plane_shader->use();
    moduleDataPtr->infinity_plane_shader->setMat4("projection", projection);
    moduleDataPtr->infinity_plane_shader->setMat4("view", view); 
    moduleDataPtr->infinity_plane_shader->setVec3("camera_world_pos", moduleDataPtr->camera->Position);

    // Draw procedural grid
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
    glBindVertexArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_BLEND);

    return 0;
}


int end_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    return 0;
}


void processInput(void * arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    GLFWwindow* window = moduleDataPtr->window;
    Camera * camera = moduleDataPtr->camera;
    // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    //     glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::UP, moduleDataPtr->deltaTime);
        // std::cout << "R Pressed" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::DOWN, moduleDataPtr->deltaTime);
        // std::cout << "F Pressed" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::FORWARD, moduleDataPtr->deltaTime);
        // std::cout << "W Pressed" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::BACKWARD, moduleDataPtr->deltaTime);
        // std::cout << "S Pressed" << std::endl;
    }
     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::LEFT, moduleDataPtr->deltaTime);
        // std::cout << "A Pressed" << std::endl;
    }    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::RIGHT, moduleDataPtr->deltaTime);
        // std::cout << "D Pressed" << std::endl;    
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::PITCH_UP, moduleDataPtr->deltaTime);
        // std::cout << "Up Pressed" << std::endl;    
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::PITCH_DOWN, moduleDataPtr->deltaTime);
        // std::cout << "Up Pressed" << std::endl;    
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::YAW_LEFT, moduleDataPtr->deltaTime);
        // std::cout << "Up Pressed" << std::endl;    
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera->ProcessKeyboard(Camera_Movement::YAW_RIGHT, moduleDataPtr->deltaTime);
        // std::cout << "Up Pressed" << std::endl;    
    }
}