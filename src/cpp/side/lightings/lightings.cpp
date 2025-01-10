#include "lightings.hpp"


extern "C" int start_module(void *arg);
extern "C" int run_module(void *arg);
extern "C" int end_module(void *arg);

int start_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*)arg;
    AppState * app_state = (AppState*) state->app_state;
    ModuleData* moduleDataPtr = (ModuleData*)malloc(sizeof(ModuleData));
    state->module_data = moduleDataPtr;


    moduleDataPtr->camera = new Camera (glm::vec3(0.0f, 0.0f, 3.0f));
    moduleDataPtr->lastX = app_state->display_width / 2.0f;
    moduleDataPtr->lastY = app_state->display_height / 2.0f;
    moduleDataPtr->firstMouse = true;
    moduleDataPtr->deltaTime = 0.0f;
    moduleDataPtr->lastFrame = 0.0f;
    moduleDataPtr->lightPos = (void *) (new glm::vec3(1.2f, 1.0f, 2.0f));
    moduleDataPtr->window = app_state->g_window;


    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    moduleDataPtr->lightingShaderID = generate_shader("multiple_lights.vs", "multiple_lights.fs");;
    state->shaderProgram = moduleDataPtr->lightingShaderID;
    std::cout << "shader program id start: " << moduleDataPtr->lightingShaderID << std::endl;
    std::cout << "shader program id start: " << ((ModuleData*)state->module_data)->lightingShaderID << std::endl;

    moduleDataPtr->lightCubeShaderID = generate_shader("light_cube.vs", "light_cube.fs");;
    

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    moduleDataPtr->cubePositions = (void*) cubePositions;
    moduleDataPtr->pointLightPositions = (void*) pointLightPositions;

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    moduleDataPtr->cubeVAO = cubeVAO;
    moduleDataPtr->VBO = VBO;
    

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    moduleDataPtr->lightCubeVAO = lightCubeVAO;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    moduleDataPtr->diffuseMap = loadTexture("container2.png");
    moduleDataPtr->specularMap = loadTexture("container2_specular.png");

    // shader configuration
    // --------------------
    use_shader(moduleDataPtr->lightingShaderID);
    set_int(moduleDataPtr->lightingShaderID,"material.diffuse", 0); 
    set_int(moduleDataPtr->lightingShaderID,"material.specular", 1); 

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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // be sure to activate shader when setting uniforms/drawing objects
    // moduleDataPtr->lightingShader.use();
    use_shader(moduleDataPtr->lightingShaderID);
    set_vec3(moduleDataPtr->lightingShaderID, "viewPos", moduleDataPtr->camera->Position);
    set_float(moduleDataPtr->lightingShaderID, "material.shininess", 32.0f);


    // directional light
    set_vec3(moduleDataPtr->lightingShaderID, "dirLight.direction", -0.2f, -1.0f, -0.3f);
    set_vec3(moduleDataPtr->lightingShaderID, "dirLight.ambient", 0.05f, 0.05f, 0.05f);
    set_vec3(moduleDataPtr->lightingShaderID, "dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    set_vec3(moduleDataPtr->lightingShaderID, "dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[0].position", ((glm::vec3*) moduleDataPtr->pointLightPositions)[0]);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[0].constant", 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[0].linear", 0.09f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[0].quadratic", 0.032f);
    // point light 2
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[1].position", ((glm::vec3*) moduleDataPtr->pointLightPositions) [1]);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[1].constant", 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[1].linear", 0.09f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[1].quadratic", 0.032f);
    // point light 3
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[2].position",((glm::vec3*) moduleDataPtr->pointLightPositions) [2]);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[2].constant", 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[2].linear", 0.09f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[2].quadratic", 0.032f);
    // point light 4
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[3].position", ((glm::vec3*) moduleDataPtr->pointLightPositions) [3]);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    set_vec3(moduleDataPtr->lightingShaderID, "pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[3].constant", 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[3].linear", 0.09f);
    set_float(moduleDataPtr->lightingShaderID, "pointLights[3].quadratic", 0.032f);
    // spotLight
    set_vec3(moduleDataPtr->lightingShaderID, "spotLight.position", moduleDataPtr->camera->Position);
    set_vec3(moduleDataPtr->lightingShaderID, "spotLight.direction", moduleDataPtr->camera->Front);
    set_vec3(moduleDataPtr->lightingShaderID, "spotLight.ambient", 0.0f, 0.0f, 0.0f);
    set_vec3(moduleDataPtr->lightingShaderID, "spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    set_vec3(moduleDataPtr->lightingShaderID, "spotLight.specular", 1.0f, 1.0f, 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "spotLight.constant", 1.0f);
    set_float(moduleDataPtr->lightingShaderID, "spotLight.linear", 0.09f);
    set_float(moduleDataPtr->lightingShaderID, "spotLight.quadratic", 0.032f);
    set_float(moduleDataPtr->lightingShaderID, "spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    set_float(moduleDataPtr->lightingShaderID, "spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));     

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(moduleDataPtr->camera->Zoom), (float) app_state->display_width / (float) app_state->canvas_height, 0.1f, 100.0f);
    glm::mat4 view = moduleDataPtr->camera->GetViewMatrix();
    set_mat4(moduleDataPtr->lightingShaderID, "projection", projection);
    set_mat4(moduleDataPtr->lightingShaderID, "view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    set_mat4(moduleDataPtr->lightingShaderID, "model", model);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, moduleDataPtr->diffuseMap);
    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, moduleDataPtr->specularMap);

    // render containers
    glBindVertexArray(moduleDataPtr->cubeVAO);
    for (unsigned int i = 0; i < 10; i++)
    {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, ((glm::vec3*)  moduleDataPtr->cubePositions)[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        set_mat4(moduleDataPtr->lightingShaderID, "model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

        // also draw the lamp object(s)
        use_shader(moduleDataPtr->lightCubeShaderID);
    //  moduleDataPtr->lightCubeShader.use();
        set_mat4(moduleDataPtr->lightCubeShaderID, "projection", projection);
        set_mat4(moduleDataPtr->lightCubeShaderID, "view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(moduleDataPtr->lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model,((glm::vec3*)  moduleDataPtr->pointLightPositions)[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            set_mat4(moduleDataPtr->lightCubeShaderID, "model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    // glfwSwapBuffers(window);
    // glfwPollEvents();

    return 0;
}

int end_module(void *arg)
{
    ModuleInterface::ModuleState* state = (ModuleInterface::ModuleState*) arg;
    ModuleData * moduleDataPtr = (ModuleData*) (state->module_data);

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &moduleDataPtr->cubeVAO);
    glDeleteVertexArrays(1, &moduleDataPtr->lightCubeVAO);
    glDeleteBuffers(1, &moduleDataPtr->VBO);
    glDeleteProgram(moduleDataPtr->lightCubeShaderID);
    glDeleteProgram(moduleDataPtr->lightingShaderID);

    free(moduleDataPtr);

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    unsigned char *data = load_png(path, &width, &height, &nrComponents);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // stbi_image_free(data);
        delete[] data;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        // stbi_image_free(data);
        delete[] data;
    }

    return textureID;
}

unsigned char* load_png(const char* filename, int* width, int* height, int* nrComponents) {
    std::vector<unsigned char> image;  // Pixel data storage
    unsigned int w, h;                 // Image dimensions

    // Decode the PNG
    unsigned int error = lodepng::decode(image, w, h, filename);
    if (error) {
        std::cerr << "Error loading PNG: " << lodepng_error_text(error) << std::endl;
        return nullptr;
    }

    // Return image dimensions and number of components
    *width = w;
    *height = h;
    *nrComponents = (unsigned int)(image.size() / (w * h));

    // Allocate and return data
    unsigned char* data = new unsigned char[image.size()];
    std::copy(image.begin(), image.end(), data);
    return data;
}
