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

    void begin_gui_frame();

    void draw_gui_data(void *arg);

    void show_list_window(void *arg);

}

#endif

#ifdef __GUI_EXTERN__

// ----- C-ABI ImGui export surface (usable by side modules) -----
#ifdef __cplusplus
extern "C" {
#endif

// Frame lifecycle (optional for side modules)
void igNewFrame(void);
void igEndFrame(void);
void igRender(void);

// Windows
void igBegin(const char* name);
void igEnd(void);

// Text & Labels
void igText(const char* text);
void igTextWrapped(const char* text);
void igLabelText(const char* label, const char* text);

// Widgets
bool igButton(const char* label);
bool igCheckbox(const char* label, bool* v);
bool igRadioButton(const char* label, bool active);
bool igSliderFloat(const char* label, float* v, float v_min, float v_max);
bool igSliderInt(const char* label, int* v, int v_min, int v_max);

bool igInputText(const char* label, char* buf, unsigned int buf_size);

// Layout
void igSameLine(void);
void igSeparator(void);
void igSpacing(void);
void igIndent(void);
void igUnindent(void);
void igSetNextWindowSize(float width, float height);
void igSetNextWindowPos(float x, float y);

// Menus & Popups
bool igBeginMenuBar(void);
void igEndMenuBar(void);
bool igBeginMenu(const char* label, bool enabled);
void igEndMenu(void);
bool igMenuItem(const char* label, const char* shortcut, bool selected, bool enabled);
void igOpenPopup(const char* str_id);
bool igBeginPopup(const char* str_id);
void igEndPopup(void);

// Tabs & Trees
bool igBeginTabBar(const char* str_id);
void igEndTabBar(void);
bool igBeginTabItem(const char* label);
void igEndTabItem(void);
bool igTreeNode(const char* label);
void igTreePop(void);
bool igCollapsingHeader(const char* label);

// Tables
bool igBeginTable(const char* str_id, int columns);
void igEndTable(void);
void igTableNextRow(void);
bool igTableNextColumn(void);
bool igTableSetColumnIndex(int column_index);

// Utilities
bool igIsItemHovered(void);
bool igIsItemClicked(void);
bool igIsWindowFocused(void);
void igGetMousePos(float* out_x, float* out_y);
void igPushID(const char* str_id);
void igPopID(void);

#ifdef __cplusplus
}
#endif
#endif

