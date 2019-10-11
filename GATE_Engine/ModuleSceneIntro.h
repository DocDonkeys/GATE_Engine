#pragma once
#include "Module.h"
#include "Globals.h"
#include "SolidSphere.h" //CHANGE/FIX if we end up having a primitives header change this

enum class tool_mode {
	DRAG,	//Q
	MOVE,	//W
	ROTATE,	//E
	SCALE,	//R
	RECT,	//T
	MULTI	//Y
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:
	//methods
	tool_mode CheckToolMode() const;

public:
	//members
	SolidSphere* sphere = nullptr;
	tool_mode currMode = tool_mode::DRAG;
};