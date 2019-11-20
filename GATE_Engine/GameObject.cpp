#include "GameObject.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Tree.h"

#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

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

void GameObject::Draw() const
{
	if (App->renderer3D->drawObjAABB)
		if (size.x != 0.f || size.y != 0.f || size.z != 0.f)
			DrawAABB();

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

void GameObject::DrawAABB() const
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	if (staticObj)
		glColor3f(0.0, 0.0, 1.0);
	else
		glColor3f(1.0, 1.0, 0.0);

	// Bottom 1
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX() + size.x, aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MinY() + size.y, aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ() + size.z);

	// Bottom 2
	glVertex3f(aabb.MaxX(), aabb.MaxY() - size.y, aabb.MaxZ());
	glVertex3f(aabb.MaxX() - size.x, aabb.MaxY() - size.y, aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY() - size.y, aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY() - size.y, aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY() - size.y, aabb.MaxZ() - size.z);

	// Top 1
	glVertex3f(aabb.MinX() + size.x, aabb.MinY() + size.y, aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MinY() + size.y, aabb.MinZ());

	glVertex3f(aabb.MinX() + size.x, aabb.MinY() + size.y, aabb.MinZ());
	glVertex3f(aabb.MinX() + size.x, aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MinX() + size.x, aabb.MinY() + size.y, aabb.MinZ());
	glVertex3f(aabb.MinX() + size.x, aabb.MinY() + size.y, aabb.MinZ() + size.z);

	// Top 2
	glVertex3f(aabb.MaxX() - size.x, aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX() - size.x, aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX() - size.x, aabb.MaxY() - size.y, aabb.MaxZ());

	glVertex3f(aabb.MaxX() - size.x, aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX() - size.x, aabb.MaxY(), aabb.MaxZ() - size.z);

	glEnd();
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(1.0f);
}

void GameObject::UpdateBoundingBox()
{
	//CHANGE/FIX: Using the mesh is wrong(?), but I don't have the time to make it better right now, it should transform with the global matrix
	ComponentMesh* mesh = (ComponentMesh*)GetComponent(COMPONENT_TYPE::MESH);
	ComponentTransform* trs = (ComponentTransform*)GetComponent(COMPONENT_TYPE::TRANSFORM);

	if (mesh != nullptr) {
		obb.SetFrom(mesh->mesh->bounds);	// Set from mesh size
		obb.Transform(trs->globalTrs);		// Transform OBB with transform global matrix
		aabb.SetFrom(obb);					// Set object AABB
		size = { abs(aabb.MaxX() - aabb.MinX()), abs(aabb.MaxY() - aabb.MinY()), abs(aabb.MaxZ() - aabb.MinZ()) };
	}
	else {	// If no mesh update based on position alone
		aabb = AABB(trs->position, trs->position);
		obb.SetFrom(aabb);
	}
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
		c = new ComponentMaterial();
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
				if (child->parent->children[i]->name == child->name)
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
