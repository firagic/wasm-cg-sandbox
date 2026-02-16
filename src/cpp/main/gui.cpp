#define __GUI__
#define __GUI_EXTERN__
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
    const char *global_font_path = "/resources/fonts/pragmasevka-regular.ttf";
    ImFont *font_18 = io.Fonts->AddFontFromFileTTF(global_font_path, 18.0f);
    ImFont *font_13 = io.Fonts->AddFontFromFileTTF(global_font_path, 13.0f);
    ImFont *font_21 = io.Fonts->AddFontFromFileTTF(global_font_path, 21.0f);
    ImFont *font_27 = io.Fonts->AddFontFromFileTTF(global_font_path, 27.0f);

    if (!font_18 || !font_13 || !font_21 || !font_27)
    {
        io.Fonts->Clear();
        io.Fonts->AddFontDefault();
    }

    // resize_canvas();

    return 0;
}

void Gui::set_gui_context()
{
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
}


void Gui::render_gui_data()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::begin_gui_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::draw_gui_data(void *arg)
{
    AppState* state = (AppState*)arg;

    
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0,0));
    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
    {
        static float f = state->data; //0.0f;
        static int counter = 0;
        ImGui::Text("Hello, world!");                                       // Display some text 
        ImGui::SliderFloat("float", &f, -10.0f, 1000.0f);                   // Edit 1 float using a slider from 0.0f to 1.0f
        // ImGui::ColorEdit3("clear color", (float *)&state->clear_color);  // Edit 3 floats representing a color

        ImGui::Checkbox("ImGUI Window", &state->show_demo_window); // Edit bools storing our windows open/close state
        ImGui::Checkbox("Examples Window", &state->show_another_window);

        if (ImGui::Button("Button")) // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Frame time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("Frame rate: %.1f FPS", ImGui::GetIO().Framerate);

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

    // ImGui::Render();
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

            bool is_loading = (is_selected && state->module_waiting_for_wasm);
            std::string item_label = state->module_names->at(i);
            item_label += is_loading ? " (loading..." + std::to_string(state->module_resource_percent_loaded) + "%)" : "";

            // Render selectable item
            // if (ImGui::Selectable(state->module_names[i], is_selected))
            if (ImGui::Selectable(item_label.c_str(), is_selected))
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

// ================================
// C-ABI ImGui export implementations
// ================================
#if defined(__EMSCRIPTEN__)
  #include <emscripten.h>
  #define KEEPALIVE EMSCRIPTEN_KEEPALIVE
#else
  #define KEEPALIVE
#endif

extern "C" {

// Frame lifecycle
KEEPALIVE void igNewFrame(void) { ImGui::NewFrame(); }
KEEPALIVE void igEndFrame(void) { ImGui::EndFrame(); }
KEEPALIVE void igRender(void)   { ImGui::Render(); }

// Windows
KEEPALIVE void igBegin(const char* name) { ImGui::Begin(name); }
KEEPALIVE void igEnd(void)               { ImGui::End(); }

// Text & Labels
KEEPALIVE void igText(const char* text)        { ImGui::Text("%s", text); }
KEEPALIVE void igTextWrapped(const char* text) { ImGui::TextWrapped("%s", text); }
KEEPALIVE void igLabelText(const char* label, const char* text) { ImGui::LabelText(label, "%s", text); }

// Widgets
KEEPALIVE bool igButton(const char* label) { return ImGui::Button(label); }
KEEPALIVE bool igCheckbox(const char* label, bool* v) { return ImGui::Checkbox(label, v); }
KEEPALIVE bool igRadioButton(const char* label, bool active) { return ImGui::RadioButton(label, active); }
KEEPALIVE bool igSliderFloat(const char* label, float* v, float v_min, float v_max) { return ImGui::SliderFloat(label, v, v_min, v_max); }
KEEPALIVE bool igSliderInt(const char* label, int* v, int v_min, int v_max) { return ImGui::SliderInt(label, v, v_min, v_max); }
KEEPALIVE bool igInputText(const char* label, char* buf, unsigned int buf_size) { return ImGui::InputText(label, buf, buf_size); }

// Layout
KEEPALIVE void igSameLine(void) { ImGui::SameLine(); }
KEEPALIVE void igSeparator(void){ ImGui::Separator(); }
KEEPALIVE void igSpacing(void)  { ImGui::Spacing(); }
KEEPALIVE void igIndent(void)   { ImGui::Indent(); }
KEEPALIVE void igUnindent(void) { ImGui::Unindent(); }
KEEPALIVE void igSetNextWindowSize(float w, float h) { ImGui::SetNextWindowSize(ImVec2(w, h)); }
KEEPALIVE void igSetNextWindowPos(float x, float y)  { ImGui::SetNextWindowPos(ImVec2(x, y)); }

// Menus & Popups
KEEPALIVE bool igBeginMenuBar(void) { return ImGui::BeginMenuBar(); }
KEEPALIVE void igEndMenuBar(void)   { ImGui::EndMenuBar(); }
KEEPALIVE bool igBeginMenu(const char* label, bool enabled) { return ImGui::BeginMenu(label, enabled); }
KEEPALIVE void igEndMenu(void)      { ImGui::EndMenu(); }
KEEPALIVE bool igMenuItem(const char* label, const char* shortcut, bool selected, bool enabled) { return ImGui::MenuItem(label, shortcut, selected, enabled); }
KEEPALIVE void igOpenPopup(const char* str_id) { ImGui::OpenPopup(str_id); }
KEEPALIVE bool igBeginPopup(const char* str_id) { return ImGui::BeginPopup(str_id); }
KEEPALIVE void igEndPopup(void) { ImGui::EndPopup(); }

// Tabs & Trees
KEEPALIVE bool igBeginTabBar(const char* str_id) { return ImGui::BeginTabBar(str_id); }
KEEPALIVE void igEndTabBar(void) { ImGui::EndTabBar(); }
KEEPALIVE bool igBeginTabItem(const char* label) { return ImGui::BeginTabItem(label); }
KEEPALIVE void igEndTabItem(void) { ImGui::EndTabItem(); }
KEEPALIVE bool igTreeNode(const char* label) { return ImGui::TreeNode(label); }
KEEPALIVE void igTreePop(void) { ImGui::TreePop(); }
KEEPALIVE bool igCollapsingHeader(const char* label) { return ImGui::CollapsingHeader(label); }

// Tables
KEEPALIVE bool igBeginTable(const char* str_id, int columns) { return ImGui::BeginTable(str_id, columns); }
KEEPALIVE void igEndTable(void) { ImGui::EndTable(); }
KEEPALIVE void igTableNextRow(void) { ImGui::TableNextRow(); }
KEEPALIVE bool igTableNextColumn(void) { return ImGui::TableNextColumn(); }
KEEPALIVE bool igTableSetColumnIndex(int column_index) { return ImGui::TableSetColumnIndex(column_index); }

// Utilities
KEEPALIVE bool igIsItemHovered(void) { return ImGui::IsItemHovered(); }
KEEPALIVE bool igIsItemClicked(void) { return ImGui::IsItemClicked(); }
KEEPALIVE bool igIsWindowFocused(void) { return ImGui::IsWindowFocused(); }
KEEPALIVE void igGetMousePos(float* out_x, float* out_y) { ImVec2 p = ImGui::GetMousePos(); *out_x = p.x; *out_y = p.y; }
KEEPALIVE void igPushID(const char* str_id) { ImGui::PushID(str_id); }
KEEPALIVE void igPopID(void) { ImGui::PopID(); }

} // extern "C"
