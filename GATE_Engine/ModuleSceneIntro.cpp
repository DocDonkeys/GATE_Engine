#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "ModuleResources.h"
#include "ModuleInput.h"
#include "TextureLoader.h"
#include "GeometryLoader.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ImporterScene.h"
#include "Tree.h"

#include "libs/SDL/include/SDL_keycode.h"
#include "libs/SDL/include/SDL_mouse.h"

#include "libs/MathGeoLib/include/Math/MathConstants.h"
#include "libs/MathGeoLib/include/Geometry/LineSegment.h"
#include "libs/MathGeoLib/include/Geometry/Triangle.h"

// Memory Leak Detection
#include "MemLeaks.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{
}

// Load assets
bool ModuleSceneIntro::Start()
{
	//LOG("Loading Intro assets");
	LOG("Loading Intro assets");
	bool ret = true;

	//Create the Root node for Game Objects
	root = new GameObject();
	ComponentTransform* trans = (ComponentTransform*)root->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trans->needsUpdateGlobal = false;

	//Setup camera
	App->camera->Move(float3(15.f, 15.f, 15.f));
	App->camera->LookAt(float3::zero);

	//Load the StreetScene
	App->resources->ImportFile("Assets\\3D_Objects\\street\\Assignment2_street.FBX");

	// Scene Tree
	staticTree = new Tree(Tree::TREE_TYPE::OC_TREE, AABB({ -50, -30.f, -50.f }, { 50.f, 30.f, 50.f }), 5);

	std::vector<const GameObject*> sceneObjects;
	GOFunctions::FillArrayWithChildren(sceneObjects, root);

	for (int i = 0; i < sceneObjects.size(); i++)
		if (sceneObjects[i]->staticObj)
			staticTree->Insert(sceneObjects[i]);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	RELEASE(root);
	RELEASE(staticTree);

	return true;
}

GameObject* ModuleSceneIntro::CreateEmptyGameObject()
{
	GameObject* go = new GameObject();
	GOFunctions::ReParentGameObject(go,root);
	return go;
}

GameObject* ModuleSceneIntro::CreateEmptyGameObject(const char* name)
{
	GameObject* go = new GameObject(name);
	GOFunctions::ReParentGameObject(go, root);
	return go;
}

GameObject* ModuleSceneIntro::CreateEmptyGameObject(COMPONENT_TYPE comp)
{
	GameObject* go = new GameObject();
	go->CreateComponent(comp);

	switch (comp) {
	case COMPONENT_TYPE::CAMERA:
		go->name = "Camera_" + std::to_string(App->scene_intro->numObjects++);
		break;
	default:
		go->name = "GameObject_" + std::to_string(App->scene_intro->numObjects++);
		break;
	}
	
	GOFunctions::ReParentGameObject(go, root);
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

	GOFunctions::ReParentGameObject(go,nullptr);
	RELEASE(go);
	numObjects--;
}

bool ModuleSceneIntro::AddTextureToGameObject(GameObject * go, const char* path)
{
	bool has_material_component = false;

	ResourceTexture* texture = App->texture_loader->LoadTextureFile(path); //TODO: Didac Make it work with module resources

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
update_status ModuleSceneIntro::PreUpdate(float realDT)
{
	root->PreUpdate(realDT);

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleSceneIntro::Update(float realDT)
{
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT)	// Avoid conflicting with First Person controls
		toolMode = CheckToolMode();

	root->Update(realDT);

	return UPDATE_CONTINUE;
}

// Game Update
update_status ModuleSceneIntro::GameUpdate(float gameDT)
{
	root->GameUpdate(gameDT);

	return UPDATE_CONTINUE;
}

// PostUpdate
update_status ModuleSceneIntro::PostUpdate(float realDT)
{
	root->PostUpdate(realDT);

	return UPDATE_CONTINUE;
}

GameObject* ModuleSceneIntro::IntersectRay(const LineSegment& segment, float& dist, bool nearest) const
{
	dist = inf;
	GameObject* chosen = nullptr;

	std::map<float, const GameObject*> objCollector;
	std::vector<const GameObject*> rootCollector;
	float nearHit, farHit;

	// Dynamic Intersection
	GOFunctions::FillArrayWithChildren(rootCollector, root);
	for (int i = 0; i < rootCollector.size(); i++)
		if (!rootCollector[i]->staticObj
			&& rootCollector[i]->active
			&& rootCollector[i]->GetComponent(COMPONENT_TYPE::MESH) != nullptr
			&& segment.Intersects(rootCollector[i]->aabb, nearHit, farHit))
			if (nearest)
				objCollector[nearHit] = rootCollector[i];
			else
				objCollector[farHit] = rootCollector[i];

	// Static Intersections
	staticTree->Intersects(objCollector, segment);

	for (std::map<float, const GameObject*>::const_iterator it = objCollector.begin(); it != objCollector.end(); it = next(it))
	{
		const GameObject* go = it->second;
		ComponentTransform* compTrs = (ComponentTransform*)go->GetComponent(COMPONENT_TYPE::TRANSFORM);
		ComponentMesh* compMesh = (ComponentMesh*)go->GetComponent(COMPONENT_TYPE::MESH);

		if (compMesh != nullptr)
		{
			ResourceMesh* mesh = compMesh->mesh;

			LineSegment seg(segment);
			seg.Transform(compTrs->GetGlobalMat().Inverted());

			Triangle tri;
			for (uint i = 0; i < mesh->num_index; i += 3)
			{
				tri.a = mesh->vertex[mesh->index[i]];
				tri.b = mesh->vertex[mesh->index[i + 1]];
				tri.c = mesh->vertex[mesh->index[i + 2]];

				float distance;
				float3 hit_point;
				if (seg.Intersects(tri, &distance, &hit_point))
				{
					if (nearest && distance < dist || !nearest && distance > dist)
					{
						dist = distance;
						chosen = (GameObject*)go;
					}
				}
			}
		}
	}

	return chosen;
}

bool ModuleSceneIntro::ChangeScene(GameObject * new_root)
{
	//Delete the current root and change it by the root of the loaded scene
	delete root;
	root = new_root;

	//TODO: Carles initialize / recalculate / delete any necessary info like quadtrees and stuff
	return false;
}

int ModuleSceneIntro::CheckToolMode()
{
	int ret = toolMode;

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN || App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN) {
		if (toolMode != (int)tool_mode::DRAG) {
			ret = (int)tool_mode::DRAG;
			lastToolMode = toolMode;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_UP && App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_IDLE
		|| App->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE && App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_UP) {
		ret = lastToolMode;
	}
	else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE && App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_IDLE) {
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
	}

	return ret;
}