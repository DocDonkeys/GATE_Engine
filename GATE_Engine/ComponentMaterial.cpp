#include "ComponentMaterial.h"
#include "Application.h"
#include "TextureLoader.h"

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

	if (checkers_texture_id == 0)
	{
		checkers_texture_id = App->texture_loader->GetDefaultTex();
	}
}

void ComponentMaterial::Update()
{
}

void ComponentMaterial::Disable()
{
	active = false;
}
