#include "Application.h"
#include "ModulePhysics.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

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