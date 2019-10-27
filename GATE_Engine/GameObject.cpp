#include "GameObject.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

GameObject::GameObject()
{
	name = "GameObject_" + std::to_string(App->scene_intro->game_objects.size());
	
	//All Game Objects must have a transform component, so we assign it from creation
	CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::GameObject(const char* name) : name(name)
{
	//All Game Objects must have a transform component, so we assign it from creation
	CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::~GameObject()
{
	for (int i = 0; i < children.size(); ++i)
		delete children[i];

	children.clear();

	for (int i = 0; i < components.size(); ++i)
		delete components[i];
	
	components.clear();

}

void GameObject::Update()
{
	//Update whatever the GameObject itself needs to update

	//Update the components
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->active)
			components[i]->Update();
	}
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

Component * GameObject::GetComponent(COMPONENT_TYPE type)
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
