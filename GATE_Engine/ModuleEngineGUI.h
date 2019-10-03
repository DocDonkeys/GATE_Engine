#ifndef __MODULEENGINEGUI_H__
#define __MODULEENGINEGUI_H__


#include "Globals.h"
#include "Module.h"

#include "libs/imgui-1.72b/imgui.h"
#include "libs/imgui-1.72b/imgui_internal.h"

#include "libs/imgui-1.72b/examples/imgui_impl_sdl.h"
#include "libs/imgui-1.72b/examples/imgui_impl_opengl3.h"

#include "libs/glew/include/GL/glew.h"

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

public:
	int GetWinWidth() const;
	int GetWinHeight() const;
	int GetWinScale() const;

	bool IsFullscreen() const;
	bool IsResizable() const;
	bool IsBorderless() const;
	bool IsFullDesktop() const;

public: // Vars

	// Bools to show / hide or open / close windows
	bool show_demo_window = false;
	bool show_another_window = false;
	bool show_configuration_window = false;
	bool show_console_window = false;

private:
	ImGuiIO* io = nullptr;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f); //Black

	int window_width = 1280;	//TODO: Save/Load
	int window_height = 1024;
	int window_scale = 1;

	float window_brightness = 1.0f;

	bool window_fullscreen = false;
	bool window_resizable = true;
	bool window_borderless = false;
	bool window_full_desktop = false;

};

#endif // !__MODULEENGINEGUI_H__

