#ifndef MODULEEDITOR_H
#define MODULEEDITOR_H

#include "Globals.h"
#include "Module.h"

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_internal.h"

#include "libs/imgui/examples/imgui_impl_sdl.h"
#include "libs/imgui/examples/imgui_impl_opengl3.h"

#include "libs/glew/include/GL/glew.h"

// Windows
#include "EditorConfiguration.h"
#include "EditorConsole.h"
#include "EditorGame.h"
#include "EditorHierarchy.h"
#include "EditorInspector.h"
#include "EditorProject.h"
#include "EditorScene.h"

// Elements
#include "EditorMenu.h"

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

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, const char* name = "null", bool start_enabled = true);
	virtual ~ModuleEditor();

	bool Init();
	bool Start();
	update_status Update(float dt);

public:
	// Render
	void RenderEditorUI();

	// General UI Management
	bool BeginRootWindow(char* id, bool dockSpace = true, ImGuiWindowFlags winFlags = ImGuiWindowFlags_None);	// Manual background window for Main Menu Bars and docking, requires an ImGui::End() afterwards!
	void BeginDockingSpace(char* dockSpaceId, ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_None, ImVec2 size = { 0.0f, 0.0f });

	// Modes & Radio Buttons (Note: The existence of this methods avoids code repetition and future additions only have to be made on a single spot)
	void DrawModeChange();
	void TextureModeChange();
	void ByteSizeModeChange();

public:
	// UI Windows
	EditorConfiguration editor_configuration;
	EditorConsole editor_console;
	EditorGame editor_game;
	EditorHierarchy editor_hierarchy;
	EditorInspector editor_inspector;
	EditorProject editor_project;
	EditorScene editor_scene;

	// UI Elements
	EditorMenu editor_menu;

	// Window Settings
	bool viewport_activated = false;
	bool main_dockSpace = true;

	// Debug window bools
	bool show_demo_window = false;

	// RadioButton Modes
	int drawMode = (int)draw_mode::MESH;	//IMPROVE: Each mode should be in it's correspondant module or be left here if no other seems fitting, this for example should go to Renderer3D?
	int textureMode = (int)texture_mode::TWO_D;
	int byteSizeMode = (int)byte_size_mode::KB;

	// UI usage flag
	bool using_menu = false;

	// UI values
	int menuBarHeight = 20;
	float standard_text_width = 600.0f;	//Default/max text width

	float byteAlt;
	std::string byteText;

	// UI flags
	bool show_about_window = true;

	ImGuiIO* io = nullptr;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f); //Black

};

#endif // !MODULEEDITOR_H

