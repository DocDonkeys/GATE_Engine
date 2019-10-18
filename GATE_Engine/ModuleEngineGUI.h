#ifndef __MODULEENGINEGUI_H__
#define __MODULEENGINEGUI_H__

#include "Globals.h"
#include "Module.h"

#include "libs/imgui-1.72b/imgui.h"
#include "libs/imgui-1.72b/imgui_internal.h"

#include "libs/imgui-1.72b/examples/imgui_impl_sdl.h"
#include "libs/imgui-1.72b/examples/imgui_impl_opengl3.h"

#include "libs/glew/include/GL/glew.h"

#include "Console.h"

enum class draw_mode {
	MESH = 0,
	WIREFRAME,
	VERTEX,

	MAX_MODES
};

//Mutually exclusive GL settings groups
enum class texture_mode {
	TWO_D = 0,
	CUBEMAP,

	MAX_MODES
};

class ModuleEngineGUI : public Module
{
public:
	ModuleEngineGUI(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleEngineGUI();

public:
	bool Init();
	bool Start();
	update_status Update(float dt);
	void RenderGUI();

public: // Members
	int drawMode = (int)draw_mode::MESH;
	int textureMode = (int)texture_mode::TWO_D;

	AppConsole console;

private:
	// Bools to open/close windows
	bool show_settings_window = false;
	bool show_imgui_console = false;	//CHANGE/FIX: To show_console_window
	bool show_console_window = false;	//CHANGE/FIX: Delete here and in cpp, imgui console is the same but better
	bool show_heriarchy_window = false;	//TODO
	bool show_project_window = false;	//TODO
	bool show_inspector_window = false;	//TODO
	bool show_scene_window = false;		//TODO
	bool show_game_window = false;		//TODO

	// Debug window bools
	bool show_demo_window = false;
	bool show_another_window = false;

	//UI values
	float standard_text_width = 600.0f;	//Default/max text width

	//UI flags
	bool show_about_window = true;

	ImGuiIO* io = nullptr;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f); //Black

};

#endif // !__MODULEENGINEGUI_H__

