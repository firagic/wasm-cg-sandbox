#ifdef __GUI__

#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Gui 
{

    int init_imgui(void *arg);

    void set_gui_context();

    void render_gui_data();

    void draw_gui_data(void *arg);

    void show_list_window(void *arg);

}

#endif
