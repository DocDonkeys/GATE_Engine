#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ComponentMaterial.h"

ComponentMesh::ComponentMesh() : Component()
{
	type = COMPONENT_TYPE::MESH;
}

ComponentMesh::~ComponentMesh()
{
	delete mesh;
}

void ComponentMesh::Enable()
{
	active = true;
}

void ComponentMesh::Update()
{
	Draw();
}

void ComponentMesh::Disable()
{
	active = false;
}

void ComponentMesh::Draw()
{
	ComponentMaterial* material = (ComponentMaterial*)my_go->GetComponent(COMPONENT_TYPE::MATERIAL);

	uint tex_id;
	if (material != nullptr)
		 tex_id = material->texture_id;
	else
		tex_id = 0;

	if (tex_id != 0)
		App->renderer3D->DrawMesh(mesh, tex_id);
	else
		App->renderer3D->DrawMesh(mesh);
}
