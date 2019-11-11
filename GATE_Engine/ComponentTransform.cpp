#include "ComponentTransform.h"

ComponentTransform::ComponentTransform() : Component()
{
	type = COMPONENT_TYPE::TRANSFORM;
	
	//Set Position Rotation and Scale
	position.x = position.y = position.z = 0.f;
	rotation.x = rotation.y = rotation.z = 0.f;
	scale.x = scale.y = scale.z = 1.0f;
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Enable()
{
	active = true;
}

void ComponentTransform::PreUpdate()
{

}

void ComponentTransform::Disable()
{
	active = false;
}
