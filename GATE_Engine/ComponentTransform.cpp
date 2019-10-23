#include "ComponentTransform.h"

ComponentTransform::ComponentTransform() : Component()
{
	type = COMPONENT_TYPE::TRANSFORM;
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Enable()
{
	active = true;
}

void ComponentTransform::Update()
{
	
}

void ComponentTransform::Disable()
{
	active = false;
}
