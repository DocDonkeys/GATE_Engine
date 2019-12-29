#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ImporterMesh.h"
#include "ResourceTexture.h"

#include "libs/SDL/include/SDL_opengl.h"

// Memory Leak Detection
//#include "MemLeaks.h"

ComponentMesh::ComponentMesh() : Component()
{
	type = COMPONENT_TYPE::MESH;
}

ComponentMesh::~ComponentMesh()
{
	if(mesh != nullptr)
	mesh->RemoveReference();

	mesh = nullptr;
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
	name += std::to_string(this->mesh->GetUID()).data();
	std::string path_to_save;

	imp_exp.Export(LIBRARY_MESH_FOLDER,path_to_save,&to_save,name.data());

	file["Path"] = path_to_save.data();
}

void ComponentMesh::Load(json & file)
{
	//mesh = (ResourceMesh*)App->resources->CreateNewResource(Resource::MESH);
	std::string full_path = file["Path"];
	std::string error_case = "";
	if (full_path.compare(error_case))  //If not going to crash, if the path comaprison is equal, returns 0, (false) and won't attempt to create a Resource which can't exist
	{
		//We get the resource UID from the name of the .mesh file
		std::string file_UID;
		App->file_system->SplitFilePath(full_path.data(), nullptr, &file_UID, nullptr);
		file_UID = App->SubtractString(file_UID, ".", true, true);
		file_UID = App->SubtractString(file_UID, "m", false, false);
		uint32 uid = std::stoul(file_UID);

		//If a resource with the same UID is already on resources
		mesh = (ResourceMesh*)App->resources->Get(uid);
		if (mesh != nullptr)
		{
			mesh->AddReference();
		}
		else
		{
			mesh = (ResourceMesh*)App->resources->CreateNewResource(Resource::MESH, uid);
			imp_exp.Load(full_path.data(), mesh);
		}

		ie_data.mesh = mesh;
	}
}
