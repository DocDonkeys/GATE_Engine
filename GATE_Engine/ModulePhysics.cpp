#include "Application.h"
#include "ModulePhysics.h"

// Memory Leak Detection
#include "MemLeaks.h"

ModulePhysics::ModulePhysics(Application * app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init()
{
	
	return true;
}

bool ModulePhysics::Start()
{
	return true;
}

update_status ModulePhysics::Update(float dt)
{
	
	return UPDATE_CONTINUE;
}