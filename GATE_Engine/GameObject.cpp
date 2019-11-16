#include "GameObject.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

GameObject::GameObject()
{
	UID = (uint32)App->rng.RandInt(); //CRITICAL TODO: CHANEG TO RANDOM NUMBER OF 32 bytes
	name = "GameObject_" + std::to_string(App->scene_intro->numObjects++);
	
	//All Game Objects must have a transform component, so we assign it from creation
	ComponentTransform* trans = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::GameObject(const char* name) : name(name)
{
	UID = (uint32)App->rng.RandInt(); //CRITICAL TODO: CHANEG TO RANDOM NUMBER OF 32 bytes
	//All Game Objects must have a transform component, so we assign it from creation
	CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::GameObject(const char * name, float4x4 local_mat) : name(name)
{
	UID = (uint32)App->rng.RandInt(); //CRITICAL TODO: CHANEG TO RANDOM NUMBER OF 32 bytes
	//All Game Objects must have a transform component, so we assign it from creation
	ComponentTransform* trans = (ComponentTransform*)CreateComponent(COMPONENT_TYPE::TRANSFORM);
	trans->localTrs = local_mat; //Assign the local transformation (usually used for imported objects)
}

GameObject::~GameObject()
{
	if (children.size() > 0)
	{
		for (int i = 0; i < children.size(); ++i)
		{
			delete children[i];
			children[i] = nullptr;
		}

		children.clear();
	}

	for (int i = 0; i < components.size(); ++i)
		delete components[i];
	
	components.clear();
}

void GameObject::PreUpdate()
{
	//PreUpdate whatever the GameObject itself needs to PreUpdate

	//PreUpdate the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->PreUpdate();
	}

	//Update the children game objects
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active)
			children[i]->PreUpdate();
	}
}

void GameObject::Update()
{
	//Update whatever the GameObject itself needs to Update

	//Update the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->Update();
	}

	//Update the children game objects
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active)
			children[i]->Update();
	}
}

void GameObject::PostUpdate()
{
	//PostUpdate whatever the GameObject itself needs to PostUpdate

	//PostUpdate the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->PostUpdate();
	}

	//Update the children game objects
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active)
			children[i]->PostUpdate();
	}
}

void GameObject::Draw()
{
	//Draw whatever the GameObject itself needs to draw
	if (App->renderer3D->drawObjAABB)
		DrawAABB();

	//Draw the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->Draw();
	}

	//Update the children game objects
	for (int i = 0; i < children.size(); ++i)
	{
		if (children[i]->active)
			children[i]->Draw();
	}
}

void GameObject::DrawAABB()
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 0.0);

	// Bottom 1
	glVertex3f(aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z);
	glVertex3f(aabb.minPoint.x + size.x, aabb.minPoint.y, aabb.minPoint.z);

	glVertex3f(aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z);
	glVertex3f(aabb.minPoint.x, aabb.minPoint.y + size.y, aabb.minPoint.z);

	glVertex3f(aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z);
	glVertex3f(aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z + size.z);

	// Bottom 2
	glVertex3f(aabb.maxPoint.x, aabb.maxPoint.y - size.y, aabb.maxPoint.z);
	glVertex3f(aabb.maxPoint.x - size.x, aabb.maxPoint.y - size.y, aabb.maxPoint.z);

	glVertex3f(aabb.maxPoint.x, aabb.maxPoint.y - size.y, aabb.maxPoint.z);
	glVertex3f(aabb.maxPoint.x, aabb.maxPoint.y, aabb.maxPoint.z);

	glVertex3f(aabb.maxPoint.x, aabb.maxPoint.y - size.y, aabb.maxPoint.z);
	glVertex3f(aabb.maxPoint.x, aabb.maxPoint.y - size.y, aabb.maxPoint.z - size.z);

	// Top 1
	glVertex3f(aabb.minPoint.x + size.x, aabb.minPoint.y + size.y, aabb.minPoint.z);
	glVertex3f(aabb.minPoint.x, aabb.minPoint.y + size.y, aabb.minPoint.z);

	glVertex3f(aabb.minPoint.x + size.x, aabb.minPoint.y + size.y, aabb.minPoint.z);
	glVertex3f(aabb.minPoint.x + size.x, aabb.minPoint.y, aabb.minPoint.z);

	glVertex3f(aabb.minPoint.x + size.x, aabb.minPoint.y + size.y, aabb.minPoint.z);
	glVertex3f(aabb.minPoint.x + size.x, aabb.minPoint.y + size.y, aabb.minPoint.z + size.z);

	// Top 2
	glVertex3f(aabb.maxPoint.x - size.x, aabb.maxPoint.y, aabb.maxPoint.z);
	glVertex3f(aabb.maxPoint.x, aabb.maxPoint.y, aabb.maxPoint.z);

	glVertex3f(aabb.maxPoint.x - size.x, aabb.maxPoint.y, aabb.maxPoint.z);
	glVertex3f(aabb.maxPoint.x - size.x, aabb.maxPoint.y - size.y, aabb.maxPoint.z);

	glVertex3f(aabb.maxPoint.x - size.x, aabb.maxPoint.y, aabb.maxPoint.z);
	glVertex3f(aabb.maxPoint.x - size.x, aabb.maxPoint.y, aabb.maxPoint.z - size.z);

	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(1.0f);
}

void GameObject::UpdateBoundingBox(float4x4 globalMat)
{
	//CHANGE/FIX: Using the mesh is wrong, but I don't have the time to make it better right now, it should transform with the global matrix
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(COMPONENT_TYPE::MESH);
	obb.SetFrom(mesh->mesh->bounds);

	obb.Transform(globalMat);	// Transform OBB with transform global matrix
	aabb.SetFrom(obb);			// Set object AABB
	size = { abs(aabb.maxPoint.x - aabb.minPoint.x), abs(aabb.maxPoint.y - aabb.minPoint.y), abs(aabb.maxPoint.z - aabb.minPoint.z) };
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
	default:
		break;
	}

	//Pass our our Game Object pointer to the component
	c->my_go = this;
	c->Enable();

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
				if (child->parent->children[i]->name == child->name)
				{
					child->parent->children.erase(child->parent->children.begin() + i);
					break;
				}
		}

		//Assign our new parent and add ourselves into its children list
		child->parent = new_parent;
		if (child->parent != nullptr)
			child->parent->children.push_back(child);
	}
	else {
		LOG("[Error]: A parent can't be inside the heriarchy of one of his own childs.");
	}
}

void GOFunctions::FillArrayWithChildren(std::vector<const GameObject*> &go_array, const GameObject * go, bool add_parent, uint count)
{
	if (count != 0)	//If we're not the parent that was called explicitly in the function
		go_array.push_back(go);
	else if (add_parent == true)		// In case we are the parent (Count == 0) & we want to include the parent
		go_array.push_back(go);

	count++;
	if (go->children.size() > 0)
	{
		for (int i = 0; i < go->children.size(); ++i)
		FillArrayWithChildren(go_array,go->children[i],add_parent,count);
	}
}
