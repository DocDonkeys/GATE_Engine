#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"
#include "p2Point.h"

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	//methods

public:
	//members
};