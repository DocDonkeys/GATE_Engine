#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial() : Component()
{
	type = COMPONENT_TYPE::MATERIAL;
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Enable()
{
	active = true;
}

void ComponentMaterial::Update()
{
}

void ComponentMaterial::Disable()
{
	active = false;
}
