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
	if (my_go->active && active && mesh != nullptr)
		Draw();
}

void ComponentMesh::Disable()
{
	active = false;
}

void ComponentMesh::Draw()
{
	ComponentMaterial* material = (ComponentMaterial*)my_go->GetComponent(COMPONENT_TYPE::MATERIAL);

	if (material != nullptr)
		if (material->use_default_texture) {
			App->renderer3D->DrawMesh(mesh, material->checkers_texture_id);
		}
		else if (material->active_texture != nullptr) {
			App->renderer3D->DrawMesh(mesh, material->active_texture->id);
		}
	else
		App->renderer3D->DrawMesh(mesh);

	// Debug Draw
	if (debug_vertex_normals)
		App->renderer3D->DrawMeshVertexNormals(mesh);

	if (debug_face_normals)
		App->renderer3D->DrawMeshFaceNormals(mesh);
}
