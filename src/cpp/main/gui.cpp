#define __GUI__
#include "../../hpp/globals.hpp"

/*
    GUI implementation using Dear ImGUI
*/

int Gui::init_imgui(void *arg)
{
    GLFWwindow* g_window = (GLFWwindow*)arg;

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init();

    // Setup style
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();

    // Load Fonts
    io.Fonts->AddFontFromFileTTF("/resources/fonts/xkcd-script.ttf", 23.0f);
    io.Fonts->AddFontFromFileTTF("/resources/fonts/xkcd-script.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("/resources/fonts/xkcd-script.ttf", 26.0f);
    io.Fonts->AddFontFromFileTTF("/resources/fonts/xkcd-script.ttf", 32.0f);
    io.Fonts->AddFontDefault();

    // resize_canvas();

    return 0;
}

void Gui::set_gui_context()
{
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
}


void Gui::render_gui_data()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Gui::draw_gui_data(void *arg)
{
    AppState* state = (AppState*)arg;


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
    {
        static float f = state->data; //0.0f;
        static int counter = 0;
        ImGui::Text("Hello, world!");                                       // Display some text 
        ImGui::SliderFloat("float", &f, -10.0f, 1000.0f);                   // Edit 1 float using a slider from 0.0f to 1.0f
        // ImGui::ColorEdit3("clear color", (float *)&state->clear_color);  // Edit 3 floats representing a color

        ImGui::Checkbox("Demo Window", &state->show_demo_window); // Edit bools storing our windows open/close state
        ImGui::Checkbox("Examples Window", &state->show_another_window);

        if (ImGui::Button("Button")) // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window. Using an explicit Begin/End pair to name window.
    if (state->show_another_window)
    {
        // ImGui::Begin("Another Window", &state->show_another_window);
        // ImGui::Text("Hello from another window!");
        // if (ImGui::Button("Close Me"))
        //     state->show_another_window = false;
        // ImGui::End();
        show_list_window(state);
    }

    // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
    if (state->show_demo_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowDemoWindow(&state->show_demo_window);
    }

    ImGui::Render();
}

void Gui::show_list_window(void *arg)
{
    AppState* state = (AppState*)arg;
    // json* module_list_json_obj = (json*) state->module_list;

    // Begin a new ImGui window
    ImVec2 first_window_pos = ImGui::GetWindowPos();
    ImVec2 first_window_size = ImGui::GetWindowSize();

    // Calculate the position for the second window (below the first window)
    ImVec2 second_window_pos = ImVec2(
        first_window_pos.x,                      // Same X position
        first_window_pos.y + first_window_size.y // Y position just below
    );

    // Example window
    ImGui::SetNextWindowPos(second_window_pos);
    // ImGui::SetNextWindowSize(ImVec2(300, 200));   
    ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::Begin("Examples");


    // Begin the ListBox
    if (ImGui::BeginListBox("ListBox", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
    {
        // for (int i = 0; i < module_list_json_obj->size(); i++)
        for (int i = 0; i < state->module_names->size(); i++)
        {
            // Check if item is selected
            const bool is_selected = (state->selected_item == i);
            
            // Render selectable item
            // if (ImGui::Selectable(state->module_names[i], is_selected))
            if (ImGui::Selectable(state->module_names->at(i).c_str(), is_selected))
            {
                state->selected_item = i;
                state->module_selected = true;
            }   

            // Set focus on the selected item
            if (is_selected) 
            {
                ImGui::SetItemDefaultFocus();
            }
            
        }
        ImGui::EndListBox();
    }

    // End the window
    ImGui::End();
}

