#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ImporterMesh.h"

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

void ComponentMesh::Disable()
{
	active = false;
}

void ComponentMesh::Draw()
{
	if (my_go->active && active && mesh != nullptr) {
		ComponentTransform* transform = (ComponentTransform*)my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
		ComponentMaterial* material = (ComponentMaterial*)my_go->GetComponent(COMPONENT_TYPE::MATERIAL);

		// Push Transform matrix
		glPushMatrix();
		glMultMatrixf(transform->globalTrs.Transposed().ptr());

		if (material != nullptr && material->active) {
			if (material->use_default_texture) {
				App->renderer3D->DrawMesh(mesh, material->checkers_texture_id);
			}
			else if (material->active_texture != nullptr) {
				App->renderer3D->DrawMesh(mesh, material->active_texture->id);
			}
			else {
				App->renderer3D->DrawMesh(mesh);
			}
		}
		else
			App->renderer3D->DrawMesh(mesh);

		// Debug Draw
		if (App->renderer3D->drawVertexNormals || debug_vertex_normals)
			App->renderer3D->DrawMeshVertexNormals(mesh);

		if (App->renderer3D->drawFaceNormals || debug_face_normals)
			App->renderer3D->DrawMeshFaceNormals(mesh);

		// Pop transform matrix
		glPopMatrix();
	}
}

void ComponentMesh::Save(json &file)
{
	IEMeshData to_save;
	to_save.mesh = this->mesh;

	std::string name = "_m";
	name += std::to_string(this->UID).data();
	std::string path_to_save;
	imp_exp.Export(LIBRARY_MESH_FOLDER,path_to_save,&to_save,name.data());

	file["Path"] = path_to_save.data();
}

void ComponentMesh::Load(json & file)
{
	mesh = new Mesh;

	std::string full_path = file["Path"];
	imp_exp.Load(full_path.data(), mesh);
	ie_data.mesh = mesh;
}
