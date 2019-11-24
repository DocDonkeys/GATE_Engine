#include "Importer.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneIntro.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

Importer::Importer()
{
}

Importer::~Importer()
{
}

bool Importer::Import(const char * file, const char * path, std::string & output_file, ComponentTransform * transform)
{
	bool ret = false;

	output_file = path;
	output_file += file;

	//Load a buffer to access the data of the .mesh
	char* buffer = nullptr;
	App->file_system->Load(output_file.data(), &buffer);

	if (transform == nullptr)
	{
		LOG("[WARNING] Tried to import a .trans file into a nullptr Transform component, a transform component in memory will be created");
		transform = new ComponentTransform;
	}

	//------------------- Assign data from buffer  -------------------//
	char* cursor = (char*)buffer;
	
	float3 pos_rot_scale[3];
	uint bytes = sizeof(pos_rot_scale);
	memcpy(pos_rot_scale, cursor, bytes);

	transform->SetLocalMat(pos_rot_scale[0], pos_rot_scale[1], pos_rot_scale[2]);

	float mat4x4[16];
	cursor += bytes;
	bytes = sizeof(mat4x4);
	memcpy(mat4x4,cursor,bytes);

	int k = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			transform->localTrs[i][j] = mat4x4[k];									//					Assign local matrix 
			k++;
		}

	cursor += bytes;
	bytes = sizeof(mat4x4);
	memcpy(mat4x4, cursor, bytes);

	k = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			transform->globalTrs[i][j] = mat4x4[k];								//					Assign global matrix 
			k++;
		}


	return true;
}


bool Importer::Export(const char * path, std::string & output_file, const ComponentTransform * transform, const char * filename)
{
	bool ret = false;

	uint size = 0; // This will be the size in bytes that we will need to allocate
	size += sizeof(float3) * 3  + sizeof(float4x4) * 2;

	char* data = new char[size];
	char* cursor = data;

	float3 pos_rot_scale[3] = { transform->position, transform->rotation, transform->scale };

	//---------------- Store data ----------------------//
	uint bytes = sizeof(float3) * 3;
	memcpy(cursor, pos_rot_scale,bytes);			//transformation, rotation & scale

	//Prepare the 4x4 matrices
	float local_trs[16];
	int k = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			local_trs[k] = transform->localTrs.At(i, j);
			k++;
		}

	float global_trs[16];
	k = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			global_trs[k] = transform->globalTrs.At(i, j);
			k++;
		}

	cursor += bytes;
	bytes = sizeof(local_trs); // Local Transformation
	memcpy(cursor, local_trs,bytes);

	cursor += bytes;
	bytes = sizeof(global_trs); // Global Transformation
	memcpy(cursor, global_trs, bytes);

	//Save the new .trans to disk
	ret = App->file_system->SaveUnique(output_file, data, size, path, filename, "trans");

	if (ret == true)
	{
		LOG("Succesfully exported %s to %s as a .trans", output_file, path);
	}
	else
		LOG("Failed to export %s to %s as a .trans", output_file, path);

	return ret;
}

bool Importer::Export(const char * path, std::string & output_file, const ImportExportData * ie_data, const char * filename)
{
	return false;
}

bool Importer::Load(const char * full_path)
{
	return false;
}

bool Importer::CreateMeta(const char * original_file_full_path, ImportExportData * ie_data)
{
	return false;
}

bool Importer::LoadMeta(const char * full_path)
{
	return false;
}

bool Importer::ImportableResource(const char * full_path)
{
	if (App->file_system->Exists(full_path))
		return true;
	else
		return false;
}

uint Importer::TotalGOsNum(const GameObject * go, uint& num_of_gos)
{
		num_of_gos++;

	if (go->children.size() > 0)
	{
		for (int i = 0; i < go->children.size(); ++i)
		{
			TotalGOsNum(go->children[i], num_of_gos);
		}
	}

	return num_of_gos;
}
