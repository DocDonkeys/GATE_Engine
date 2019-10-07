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

public: // Vars

	// Bools to show / hide or open / close windows	//CHANGE/FIX: Should all of them be public? There's probably going to be a lot of flags which shouldn't be able to be triggered outside it's context
	bool show_demo_window = false;
	bool show_another_window = false;
	bool show_configuration_window = false;
	bool show_console_window = false;		//CHANGE/FIX: Delete, imgui console is the same but better
	bool show_imgui_console = false;
	bool show_about_window = true;

	AppConsole console;

	//UI values
	float standard_text_width = 600.0f;	//Default/max text width

private:
	ImGuiIO* io = nullptr;
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f); //Black

};

#endif // !__MODULEENGINEGUI_H__

