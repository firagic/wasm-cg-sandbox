#include "process_model.hpp"

int start_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*)arg;
    AppState * app_state = (AppState*) state->app_state;
    ModuleData * moduleDataPtr = new ModuleData() ;
    state->module_data = moduleDataPtr;

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    moduleDataPtr->obj_shader = new Shader("model_loading.vs", "model_loading.fs");
    moduleDataPtr->obj_model = new Model("backpack.obj");

    moduleDataPtr->camera = new Camera (glm::vec3(0.0f, 0.0f, 3.0f));
    moduleDataPtr->lastX = app_state->display_width / 2.0f;
    moduleDataPtr->lastY = app_state->display_height / 2.0f;
    moduleDataPtr->firstMouse = true;
    moduleDataPtr->deltaTime = 0.0f;
    moduleDataPtr->lastFrame = 0.0f;
    moduleDataPtr->window = app_state->g_window;


    return 0;

}

int run_module(void *arg) 
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    AppState * app_state = (AppState*) state->app_state;

    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    // render loop
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
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(moduleDataPtr->camera->Zoom), (float)app_state->canvas_width/ (float)app_state->canvas_height, 0.1f, 100.0f);
    glm::mat4 view = moduleDataPtr->camera->GetViewMatrix();
    moduleDataPtr->obj_shader->use();
    moduleDataPtr->obj_shader->setMat4("projection", projection);
    moduleDataPtr->obj_shader->setMat4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    moduleDataPtr->obj_shader->setMat4("model", model);
    moduleDataPtr->obj_model->Draw(*moduleDataPtr->obj_shader);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(moduleDataPtr->window);
    glfwPollEvents(); 



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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->ProcessKeyboard(FORWARD, moduleDataPtr->deltaTime);
        // std::cout << "W Pressed" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->ProcessKeyboard(BACKWARD, moduleDataPtr->deltaTime);
        // std::cout << "S Pressed" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->ProcessKeyboard(LEFT, moduleDataPtr->deltaTime);
        // std::cout << "A Pressed" << std::endl;
    }    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->ProcessKeyboard(RIGHT, moduleDataPtr->deltaTime);
        // std::cout << "D Pressed" << std::endl;    
    }
}