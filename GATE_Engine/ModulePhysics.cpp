#include "Application.h"
#include "ModulePhysics.h"

ModulePhysics::ModulePhysics(Application * app, bool start_enabled) : Module(app, start_enabled)
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