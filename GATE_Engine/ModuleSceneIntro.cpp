#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GeometryLoader.h"
#include <math.h>
#include <ctime>
#include "Application.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

ModuleSceneIntro::ModuleSceneIntro(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	//LOG("Loading Intro assets");
	LOG("Loading Intro assets");
	bool ret = true;

	//Rand seed based on current time
	srand((uint)time(NULL));

	//Setup camera
	App->camera->Move(vec3(1.0f, 1.0f, 1.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	currMode = CheckToolMode();

	//Ground Render	(Used the Primitives Container)
	/*Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();*/

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleSceneIntro::PostUpdate(float dt)
{
	//Ground Render	(Used the Primitives Container)
	/*Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();*/

	//sphere->draw(0,0,0);

	return UPDATE_CONTINUE;
}

tool_mode ModuleSceneIntro::CheckToolMode() const
{
	tool_mode ret = currMode;

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		ret = tool_mode::DRAG;
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		ret = tool_mode::MOVE;
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		ret = tool_mode::ROTATE;
	else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		ret = tool_mode::SCALE;
	else if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		ret = tool_mode::RECT;
	else if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
		ret = tool_mode::MULTI;

	return ret;
}