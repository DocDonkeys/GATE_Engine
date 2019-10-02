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
	ModuleEngineGUI(Application* app, bool start_enabled = true);
	~ModuleEngineGUI();

public:
	bool Init();
	bool Start();
	update_status Update(float dt);

	// Bools to show / hide or open / close windows
	bool show_demo_window = false;
	bool show_another_window = false;
	bool show_configuration_window = true;
	bool show_console_window = true;

	

private:
	ImGuiIO* io = nullptr;

	int window_width = 1280; // TEST
	int window_height = 1920; // TEST

	float window_brightness = 1.0f;

	bool window_fullscreen = false;
	bool window_resizable = true;
	bool window_borderless = false;
	bool window_full_desktop = false;

};

#endif // !__MODULEENGINEGUI_H__

