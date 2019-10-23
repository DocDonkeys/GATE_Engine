#include "ComponentMesh.h"

ComponentMesh::ComponentMesh() : Component()
{
	type = COMPONENT_TYPE::MESH;
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Enable()
{
	active = true;
}

void ComponentMesh::Update()
{
}

void ComponentMesh::Disable()
{
	active = false;
}
