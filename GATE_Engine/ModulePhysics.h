#ifndef PHYSICS_H
#define PHYSICS_H

#include "libs/MathGeoLib/include/MathGeoLib.h"	//Namespace "math::" for lib usage
#ifdef _DEBUG
#pragma comment (lib, "libs/MathGeoLib/libx86/Debug/MathGeoLib.lib")
#else
#pragma comment (lib, "libs/MathGeoLib/libx86/Release/MathGeoLib.lib")
#endif

#include "Module.h"
#include "Globals.h"

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, const char* name = "null", bool start_enabled = true);
	~ModulePhysics();

public:
	bool Init();
	bool Start();
	update_status Update(float dt);

};

#endif // PHYSICS_H