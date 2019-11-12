#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

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
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	GameObject* CreateEmptyGameObject();
	GameObject* CreateEmptyGameObject(const char* name, float4x4 local_transform);
	void CreateEmptyGameObject(int num_of_go);
	void DestroyGameObject(GameObject* go);

	bool  AddTextureToGameObject(GameObject* go, const char* path);

public:
	//methods
	int CheckToolMode() const;

public:
	//members
	int toolMode = (int)tool_mode::DRAG;	// CHANGE/FIX: Save&Load

	uint numObjects = 0;
	GameObject* root = nullptr;
	GameObject* selected_go = nullptr;
};