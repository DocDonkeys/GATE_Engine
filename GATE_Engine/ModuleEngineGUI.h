#ifndef __MODULEENGINEGUI_H__
#define __MODULEENGINEGUI_H__


#include "Globals.h"
#include "Module.h"

#include "imgui-1.72b/imgui.h"

#include "imgui-1.72b/examples/imgui_impl_sdl.h"
#include "imgui-1.72b/examples/imgui_impl_opengl2.h"
//#include "imgui-1.72b/examples/libs/glfw/include/GLFW/glfw3.h"
//#include "imgui-1.72b/examples/libs/glfw/include/GLFW/glfw3native.h"


class ModuleEngineGUI : public Module
{
public:
	// CONSTRUCTOR
	ModuleEngineGUI(Application* app, bool start_enabled = true);
	//DESTRUCTOR
	~ModuleEngineGUI();

public:
	bool Init();
	bool Start();
	update_status Update(float dt);

};

#endif // !__MODULEENGINEGUI_H__

