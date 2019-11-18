#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GeometryLoader.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ImporterScene.h"

#include <math.h>

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

	//Create the Root node for Game Objects
	root = new GameObject();
	ComponentTransform* trans = (ComponentTransform*)root->GetComponent(COMPONENT_TYPE::TRANSFORM);

	//Setup camera
	App->camera->Move(float3(15.0f, 15.0f, 15.0f));
	App->camera->LookAt(float3(0.f, 0.f, 0.f));

	//TEST: Import scene
	ImporterScene importer;
	const char* full_path = "settings/scene_test.json";
	importer.Load(full_path);

	//Load the Baker House
	App->geometry_loader->Load3DFile("Assets\\3D_Objects\\Baker_house\\BakerHouse.fbx");

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	//TEST: Save a scene
	ImporterScene scene_imp;
	std::string scene_name = "scene_test";
	scene_imp.SaveScene(root,scene_name,FileType::SCENE);

	delete root;

	return true;
}

GameObject* ModuleSceneIntro::CreateEmptyGameObject()
{
	GameObject* go = new GameObject();
	return go;
}

GameObject* ModuleSceneIntro::CreateEmptyGameObject(const char* name, float4x4 local_transform)
{
	GameObject* go = new GameObject(name,local_transform);
	return go;
}

void ModuleSceneIntro::CreateEmptyGameObject(int num_of_go)
{
	for (int i = 0; i < num_of_go; ++i)
	{
		CreateEmptyGameObject();
	}
}

void ModuleSceneIntro::DestroyGameObject(GameObject * go)
{
	//Set all possible references to the gaemObject to nullptr or out of the array
	if (selected_go == go)
		selected_go = nullptr;

	/*for (int i = 0; i < game_objects.size(); ++i)
	{
		if (game_objects[i] == go)
		{
			game_objects.erase(game_objects.begin() + i);
			break;
		}	
	}*/

	delete go;
	numObjects--;
}

bool ModuleSceneIntro::AddTextureToGameObject(GameObject * go, const char* path)
{
	bool has_material_component = false;

	Texture* texture = App->texture_loader->LoadTextureFile(path);

	for (int i = 0; i < go->components.size(); ++i)
	{
		if (go->components[i]->type == COMPONENT_TYPE::MATERIAL)
		{
			ComponentMaterial* material = (ComponentMaterial*)go->GetComponent(COMPONENT_TYPE::MATERIAL);
			material->AssignTexture(texture);
			return true;
		}
	}

	if (!has_material_component)
	{
		ComponentMaterial* material = (ComponentMaterial*)go->CreateComponent(COMPONENT_TYPE::MATERIAL);
		material->AssignTexture(texture);
		return true;
	}

	return false;
}

// PreUpdate
update_status ModuleSceneIntro::PreUpdate(float dt)
{
	root->PreUpdate();

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT)	// Avoid conflicting with First Person controls
		toolMode = CheckToolMode();

	root->Update();

	//Ground Render	(Used the Primitives Container)
	/*Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();*/

	return UPDATE_CONTINUE;
}

// PostUpdate
update_status ModuleSceneIntro::PostUpdate(float dt)
{
	root->PostUpdate();

	return UPDATE_CONTINUE;
}

int ModuleSceneIntro::CheckToolMode() const
{
	int ret = toolMode;

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		ret = (int)tool_mode::DRAG;
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		ret = (int)tool_mode::MOVE;
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		ret = (int)tool_mode::ROTATE;
	else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		ret = (int)tool_mode::SCALE;
	else if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
		ret = (int)tool_mode::RECT;
	else if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN && App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_IDLE)	// To avoid calling on Redo (CTRL+Y)
		ret = (int)tool_mode::MULTI;

	return ret;
}