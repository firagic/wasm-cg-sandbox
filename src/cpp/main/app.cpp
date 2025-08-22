#define __APP__
#define __GUI__
#define __EMJS__
#define __MODULE_INTERFACE__

#include "../../hpp/globals.hpp"



void App::on_size_changed(void *arg) 
{    
    AppState* state = (AppState*)arg;
    
    state->canvas_width = EMJS::canvas_get_width();
    state->canvas_height = EMJS::canvas_get_height();
    if (state->canvas_width != state->g_width || state->canvas_height != state->g_height) {
        state->g_width = state->canvas_width;
        state->g_height = state->canvas_height;
        glfwSetWindowSize(state->g_window, state->g_width, state->g_height);
        Gui::set_gui_context();
    }

}

void App::loop(void *arg) 
{
    AppState* state = (AppState*)arg;

    on_size_changed(state);

    glfwPollEvents();
    glfwMakeContextCurrent(state->g_window);
    glfwGetFramebufferSize(state->g_window, &state->display_width, &state->display_height);

    glViewport(0, 0, state->display_width, state->display_height);
    // glClearColor(state->clear_color.x, state->clear_color.y, state->clear_color.z, state->clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    Gui::begin_gui_frame();

    ModuleInterface::exec_module(state);

    Gui::draw_gui_data(state);
    Gui::render_gui_data();
    

    glfwMakeContextCurrent(state->g_window);
}

int App::init_modules(void *arg) 
{
    AppState* state = (AppState*)arg;
    while (!EMJS::check_read_flag()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep 100ms to avoid busy waiting
    }; // wait for modules.json to be done reading

    std::string module_list_json_str = EMJS::get_modules_json_data();
    json *module_list_json_object = new json();
    *module_list_json_object = json::parse(module_list_json_str);
    // std::cout << "READ IN " << (*module_list_json_object).size() << std::endl;

    state->module_wasms = new std::vector<std::string>();
    state->module_names = new std::vector<std::string>();

    for (int i = 0; i < (*module_list_json_object).size(); i++) {
        state->module_names->push_back((*module_list_json_object)[i]["name"]);
        state->module_wasms->push_back((*module_list_json_object)[i]["data"]["asset"][0]);
    }

    state->module_list = (void*) module_list_json_object;
    state->module_selected = false;
    return 0;

}

int App::init_gl(void *arg)
{
    AppState* state = (AppState*)arg;
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    // state->clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    int canvasWidth = state->g_width;
    int canvasHeight = state->g_height;
    state->g_window = glfwCreateWindow(canvasWidth, canvasHeight, "ImGUI Web Demo", NULL, NULL);
    if (state->g_window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(state->g_window); // Initialize GLEW

    return 0;
}


int App::init(void *arg)
{
    AppState* state = (AppState*)arg;
    init_gl(state);
    init_modules(state);
    Gui::init_imgui(state->g_window);
    EMJS::resize_canvas();
    return 0;
}

void App::quit()
{
    glfwTerminate();
}


void App::run(int argc, char **argv)
{
    // Create the state and initialize it
    AppState* state = (AppState*)malloc(sizeof(AppState));
    state->g_width = EMJS::canvas_get_width();
    state->g_height = EMJS::canvas_get_height();
    if (init(state) != 0)
        return;

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(loop, state, 0, 1);
    #endif

    quit();
    return;
}

