#include "ModuleEngineGUI.h"

ModuleEngineGUI::ModuleEngineGUI(Application * app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleEngineGUI::~ModuleEngineGUI()
{
}

bool ModuleEngineGUI::Init()
{
	return true;
}

bool ModuleEngineGUI::Start()
{
	return true;
}

update_status ModuleEngineGUI::Update(float dt)
{
	return UPDATE_CONTINUE;
}
