#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include <math.h>
#include <ctime>
#include "Application.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	//LOG("Loading Intro assets");
	App->ConsoleLOG("Loading Intro assets");
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
	App->ConsoleLOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Ground Render	(Used the Primitives Container)
	/*Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();*/

	/*if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {
		App->RequestLoad();
	}*/

	return UPDATE_CONTINUE;
}