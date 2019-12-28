#include "GameObject.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleScripting.h"
#include "Tree.h"

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "ComponentScript.h"

#include "libs/SDL/include/SDL_opengl.h"

// Memory Leak Detection
#include "MemLeaks.h"

GameObject::GameObject() : size(float3::zero)
{
	UID = App->rng.RandInt<uint32_t>();
	name = "GameObject_" + std::to_string(App->scene_intro->numObjects++);
	
	//All Game Objects must have a transform component, so we assign it from creation
	CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::GameObject(const char* name) : name(name), size(float3::zero)
{
	UID = App->rng.RandInt<uint32_t>();
	//All Game Objects must have a transform component, so we assign it from creation
	CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::~GameObject()
{
	if (children.size() > 0)
	{
		for (int i = 0; i < children.size(); ++i)
			RELEASE(children[i]);

		children.clear();
	}

	for (int i = 0; i < components.size(); ++i)
		RELEASE(components[i]);
	
	components.clear();
}

void GameObject::PreUpdate(float realDT)
{
	//PreUpdate whatever the GameObject itself needs to PreUpdate

	//PreUpdate the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->PreUpdate(realDT);
	}

	//Update the children game objects
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active)
			children[i]->PreUpdate(realDT);
	}
}

void GameObject::Update(float realDT)
{
	//Update whatever the GameObject itself needs to Update

	//Update the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->Update(realDT);
	}

	//Update the children game objects
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active)
			children[i]->Update(realDT);
	}
}

void GameObject::GameUpdate(float gameDT)
{
	//Update whatever the GameObject itself needs to Update

	//Update the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->GameUpdate(gameDT);
	}

	//Update the children game objects
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active)
			children[i]->GameUpdate(gameDT);
	}
}

void GameObject::PostUpdate(float realDT)
{
	//PostUpdate whatever the GameObject itself needs to PostUpdate

	//PostUpdate the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->PostUpdate(realDT);
	}

	//Update the children game objects
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active)
			children[i]->PostUpdate(realDT);
	}
}

void GameObject::Draw() const
{
	if (App->renderer3D->drawObjAABB)
		if (!size.IsZero()) {
			if (staticObj)
				DrawAABB(aabb, float3(0.f, 0.f, 1.f));
			else
				DrawAABB(aabb, float3(1.f, 1.f, 0.f));
		}

	//Draw the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->Draw();
	}

	//Update the children game objects
	/*for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active && App->camera->Intersects(children[i]->aabb))
			children[i]->Draw();
	}*/
}

void GameObject::DrawAABB(const AABB& aabb, const float3& rgb)
{
	glLineWidth(2.0f);
	glColor3f(rgb.x, rgb.y, rgb.z);
	glBegin(GL_LINES);

	// Bottom 1
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	// Bottom 2
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	// Top 1
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());

	// Top 2
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glEnd();
	glColor3f(1.f, 1.f, 1.f);
	glLineWidth(1.0f);
}

void GameObject::UpdateBoundingBox()
{
	//CHANGE/FIX: Using the mesh is wrong(?), but I don't have the time to make it better right now, it should transform with the global matrix
	ComponentTransform* trs = (ComponentTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM);
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(COMPONENT_TYPE::MESH);
	ComponentCamera* cam = (ComponentCamera*)GetComponent(COMPONENT_TYPE::CAMERA);

	if (mesh != nullptr && mesh->mesh != nullptr) {
		obb.SetFrom(mesh->mesh->bounds);	// Set from mesh size
		obb.Transform(trs->globalTrs);		// Transform OBB with transform global matrix
		aabb.SetFrom(obb);					// Set object AABB
		size = { abs(aabb.MaxX() - aabb.MinX()), abs(aabb.MaxY() - aabb.MinY()), abs(aabb.MaxZ() - aabb.MinZ()) };
	}
	else if (cam != nullptr) {
		aabb.SetFrom(AABB(trs->position - float3::one / 2.f, trs->position + float3::one / 2.f));
		size = { abs(aabb.MaxX() - aabb.MinX()), abs(aabb.MaxY() - aabb.MinY()), abs(aabb.MaxZ() - aabb.MinZ()) };
	}
	else {	// If no mesh update based on position alone
		aabb = AABB(trs->position, trs->position);
		obb.SetFrom(aabb);
	}
}

void GameObject::UpdateChildrenActive(bool newVal)
{
	active = newVal;

	for (int i = 0; i < children.size(); i++)
		children[i]->UpdateChildrenActive(newVal);
}

void GameObject::UpdateStaticStatus(bool newVal, bool recursive)
{
	staticObj = newVal;

	if (newVal)
		App->scene_intro->staticTree->Insert(this);
	else
		App->scene_intro->staticTree->Remove(this);

	if (recursive)
		for (int i = 0; i < children.size(); i++)
			children[i]->UpdateStaticStatus(newVal);
}

uint32 GameObject::ChangeRandomUID()
{
	uint32 ret = App->rng.RandInt<uint32_t>();

	this->UID = ret;

	for (int i = 0; i < this->children.size(); ++i)
	{
		this->children[i]->parent_UID = ret;
	}

	return ret;
}


Component * GameObject::CreateComponent(COMPONENT_TYPE type)
{
	Component* c = nullptr;

	switch (type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		c = new ComponentTransform();
		break;
	case COMPONENT_TYPE::MESH:
		c = new ComponentMesh();
		break;
	case COMPONENT_TYPE::MATERIAL:
		c = new ComponentMaterial();
		break;
	case COMPONENT_TYPE::CAMERA:
		c = new ComponentCamera();
		break;
	case COMPONENT_TYPE::SCRIPT:
		c = new ComponentScript();
		break;
	default:
		break;
	}

	//Pass our our Game Object pointer to the component
	c->my_go = this;
	c->Enable();
	c->UID = App->rng.RandInt<uint32_t>();

	components.push_back(c);
	return c;
}

Component * GameObject::GetComponent(COMPONENT_TYPE type) const
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->type == type)
		{
			return components[i];
		}
	}
	return nullptr;
}

void GOFunctions::ReParentGameObject(GameObject * child, GameObject * new_parent)
{
	GameObject* it = new_parent;
	bool ret = false;

	while (it != nullptr && it != App->scene_intro->root) {	// Check if child is parent of new_parent
		if (it == child) {
			ret = true;
			break;
		}
		else
			it = it->parent;
	}

	if (!ret) {
		//Get ourselves out of our current parent's children list
		if (child->parent != nullptr) {

			for (int i = 0; i < child->parent->children.size(); ++i)
				if (child->parent->children[i] == child)
				{
					child->parent->children.erase(child->parent->children.begin() + i);
					break;
				}
		}

		//Assign our new parent and add ourselves into its children list
		child->parent = new_parent;
		if (child->parent != nullptr)
		{
			child->parent_UID = child->parent->UID;
			child->parent->children.push_back(child);
		}
		else
			child->parent_UID = 0;
	}
	else {
		LOG("[Error]: A parent can't be inside the hierarchy of one of his own childs.");
	}
}

void GOFunctions::FillArrayWithChildren(std::vector<const GameObject*> &go_array, const GameObject * go, bool add_parent, uint count)
{
	if (go != nullptr)
	{
		if (count != 0)	//If we're not the parent that was called explicitly in the function
			go_array.push_back(go);
		else if (add_parent == true)		// In case we are the parent (Count == 0) & we want to include the parent
			go_array.push_back(go);

		count++;
		if (go->children.size() > 0)
		{
			for (int i = 0; i < go->children.size(); ++i)
				FillArrayWithChildren(go_array, go->children[i], add_parent, count);
		}
	}
}

void GOFunctions::FillArrayWithModifiableChildren(std::vector<GameObject*> &go_array, GameObject * go, bool add_parent, uint count)
{
	if (go != nullptr)
	{
		if (count != 0)	//If we're not the parent that was called explicitly in the function
			go_array.push_back(go);
		else if (add_parent == true)		// In case we are the parent (Count == 0) & we want to include the parent
			go_array.push_back(go);

		count++;
		if (go->children.size() > 0)
		{
			for (int i = 0; i < go->children.size(); ++i)
				FillArrayWithModifiableChildren(go_array, go->children[i], add_parent, count);
		}
	}
}

GameObject * GOFunctions::InstantiateGameObject(GameObject * go_to_instantiate)
{
	GameObject* ret = nullptr;

	std::vector<GameObject*> gos;
	FillArrayWithModifiableChildren(gos, go_to_instantiate, true);

	std::vector<GameObject*> instance_gos;
	
	//Copy all gameObjects in new GameOBjects, later on we will reparent them, change the ID and repeat the process for each GamObject Component
	//We do this process in this way because we need to copy by value dereferencing each go and later on its components to change ID's and have a completely new copy without any trouble with the engine
	for (int i = 0; i < gos.size(); ++i)
	{
		GameObject* new_go = new GameObject();
		*new_go = *gos[i];	//Copy by reference the gameObject
		new_go->children.clear(); //Don't keep any pointer! we will replenish this accordingly like we do with LoadScene!
		new_go->components.clear();

		std::vector<COMPONENT_TYPE> component_types;

		for (int j = 0; j < gos[i]->components.size(); ++j)
		{
			 ComponentTransform* trans;
			 ComponentMesh* mesh;
			 ComponentMaterial* material;
			 ComponentCamera* camera;
			 ComponentScript* script;

			 std::string actually = "this is no longer used so whatever, will remove after programming, time is pretty tight";

			 switch (gos[i]->components[j]->type)
			 {
			 case COMPONENT_TYPE::TRANSFORM:
				 trans = new ComponentTransform();
				 *trans = *(ComponentTransform*)gos[i]->components[j];
				 trans->my_go = new_go;
				 trans->UID = App->rng.RandInt<uint32_t>();
				 new_go->components.push_back(trans);
				 break;
			 case COMPONENT_TYPE::MESH:
				 mesh = new ComponentMesh();
				 *mesh = *(ComponentMesh*)gos[i]->components[j];
				 mesh->my_go = new_go;
				 mesh->UID = App->rng.RandInt<uint32_t>();
				 new_go->components.push_back(mesh);
				 break;
			 case COMPONENT_TYPE::MATERIAL:
				 material = new ComponentMaterial();
				 *material = *(ComponentMaterial*)gos[i]->components[j];
				 material->my_go = new_go;
				 material->UID = App->rng.RandInt<uint32_t>();
				 new_go->components.push_back(material);
				 break;
			 case COMPONENT_TYPE::CAMERA:
				camera = new ComponentCamera();
				 *camera = *(ComponentCamera*)gos[i]->components[j];
				 camera->my_go = new_go;
				 camera->UID = App->rng.RandInt<uint32_t>();
				 new_go->components.push_back(camera);
				 break;
			 case COMPONENT_TYPE::SCRIPT:
				 script = new ComponentScript();
				 *script = *(ComponentScript*)gos[i]->components[j];
				 script->my_go = new_go;
				 script->UID = App->rng.RandInt<uint32_t>();
				 new_go->components.push_back(script);

				 //In the case of the script we have to do a couple of extra things, (getting a new instance in the virtual machine)
				 App->scripting->SendScriptToModule(script, actually);
				 break;
			 default:
				 break;
			 }
						
		}
		//Push back the gameObject
		instance_gos.push_back(new_go);
	}

	for (int i = 0; i < instance_gos.size(); ++i)
	{
		if (i == 0)
		{
				GOFunctions::ReParentGameObject(instance_gos[i], App->scene_intro->root);
		}
		else
		{
			for (int j = 0; j < instance_gos.size(); ++j)
			{
				if (instance_gos[j]->UID == instance_gos[i]->parent_UID)
				{
					GOFunctions::ReParentGameObject(instance_gos[i], instance_gos[j]);
					break;
				}
			}
		}
	}

	//Now we will change the name to instance, remmeber an instance must only be made under gameplay, or else when trying to save & load the scene
	// or prefab, we are just using name as parameter in the .json (no time to change it tu use UID's D: so components would get reeeeally fucked up)
	//Change the GameObject's UIDS
	instance_gos[0]->name += "_instance_";
	for (int i = 0; i < instance_gos.size(); ++i)
	{
		instance_gos[i]->ChangeRandomUID();
	}
		
	ret = instance_gos[0];
	return ret;
}


