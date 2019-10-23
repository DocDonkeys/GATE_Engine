#ifndef __MODULEENGINEGUI_H__
#define __MODULEENGINEGUI_H__

#include "Globals.h"
#include "Module.h"

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_internal.h"

#include "libs/imgui/examples/imgui_impl_sdl.h"
#include "libs/imgui/examples/imgui_impl_opengl3.h"

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

enum class byte_size_mode {
	KB,
	MB,
	GB,

	MAX_MODES
};

class ModuleEngineGUI : public Module
{
public:
	ModuleEngineGUI(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleEngineGUI();

	bool Init();
	bool Start();
	update_status Update(float dt);

public:
	void RenderGUI();

private:
	// Modes & Radio Buttons (Note: The existence of this methods avoids code repetition and future additions only have to be made on a single spot)
	void DrawModeChange();
	void TextureModeChange();
	void ByteSizeModeChange();

	bool BeginRootWindow(char* id, bool dockSpace = true, ImGuiWindowFlags winFlags = ImGuiWindowFlags_None);	// Manual background window for Main Menu Bars and docking, requires an ImGui::End() afterwards!
	void BeginDockingSpace(char* dockSpaceId, ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_None, ImVec2 size = { 0.0f, 0.0f });

public: // Members
	int drawMode = (int)draw_mode::MESH;	//IMPROVE: Each mode should be in it's correspondant module or be left here if no other seems fitting, this for example should go to Renderer3D?
	int textureMode = (int)texture_mode::TWO_D;
	int byteSizeMode = (int)byte_size_mode::KB;

	bool using_menu = false;

	AppConsole console;

private:
	// Bools to open/close windows
	bool show_settings_window = false;
	bool show_console_window = true;	//CHANGE/FIX: Initial values should be in SAVE&LOAD
	bool show_hierarchy_window = true;	//TODO
	bool show_project_window = false;	//TODO
	bool show_inspector_window = true;	//TODO
	bool show_scene_window = false;		//TODO
	bool show_game_window = false;		//TODO

	// Window Settings
	bool viewport_activated = false;
	bool main_dockSpace = true;

	// Debug window bools
	bool show_demo_window = false;
	bool show_another_window = false;

	//UI values
	int menuBarHeight = 20;
	float standard_text_width = 600.0f;	//Default/max text width

	float byteAlt;
	std::string byteText;

	//UI flags
	bool show_about_window = true;

	ImGuiIO* io = nullptr;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f); //Black

};

#endif // !__MODULEENGINEGUI_H__

