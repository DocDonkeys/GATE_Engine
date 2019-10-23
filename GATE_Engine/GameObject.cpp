#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

GameObject::GameObject()
{
	name = "GameObject";

	//All Game Objects must have a transform component, so we assign it from creation
	CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

GameObject::~GameObject()
{
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
