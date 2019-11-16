#include "Importer.h"
#include "Mesh.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"

Importer::Importer()
{
}

Importer::~Importer()
{
}

bool Importer::Import(const char * file, const char * path, std::string & output_file, Mesh* mesh)
{
	bool ret = false;
	output_file = path;
	output_file += file;

	//Load a buffer to access the data of the .mesh
	char* buffer = nullptr;
	App->file_system->Load(output_file.data(), &buffer);

	if (mesh == nullptr)
	{
		LOG("[WARNING] Tried to import a .mesh file into a nullptr mesh, a mesh will be created in memory");
		mesh = new Mesh;
	}

	//------------------- Assign data from buffer  -------------------//
	char* cursor = (char*)buffer;
	// amount of indices / vertices / colors / normals / texture_coords
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);

	mesh->num_index = ranges[0];
	mesh->num_vertex = ranges[1];
	mesh->num_normals = ranges[2];
	mesh->num_tex_coords = ranges[3];

	cursor += bytes;																		//																//
	bytes = sizeof(uint) * mesh->num_index;									//					Copy Indices 						//
	mesh->index = new uint[mesh->num_index];
	memcpy(mesh->index, cursor, bytes);										//																//

	cursor += bytes;																		//																//
	bytes = sizeof(float) * mesh->num_vertex * 3;						//					Copy Vertices   					//
	mesh->vertex = new float3[mesh->num_vertex];
	memcpy(mesh->vertex, cursor, bytes);										//																//

	cursor += bytes;																		//																//
	bytes = sizeof(float) * mesh->num_normals * 3;						//					Copy Normals Vector   			//
	mesh->normals_vector = new float3[mesh->num_normals];
	memcpy(mesh->normals_vector, cursor, bytes);						//																//

	cursor += bytes;																		//																//
	bytes = sizeof(float) * mesh->num_normals * 3;						//					Copy Normals Faces  			//
	mesh->normals_faces = new float3[mesh->num_normals];		//																//
	memcpy(mesh->normals_faces, cursor, bytes);							//																//

	cursor += bytes;																							//																					//
	bytes = sizeof(float) * mesh->num_normals * 3;											//					Copy Normals Faces vectors   					//
	mesh->normals_faces_vector = new float3[mesh->num_normals];				//																					//
	memcpy(mesh->normals_faces_vector, cursor, bytes);								//																					//
	
	cursor += bytes;																							//																					//
	bytes = sizeof(float) * mesh->num_tex_coords * 2;										//					Copy Texture Coordinates						//
	mesh->tex_coords = new float[mesh->num_tex_coords * 2];						//																					//
	memcpy(mesh->tex_coords, cursor, bytes);													//																					//

	return ret;
}

bool Importer::Import(const char* full_path, Mesh * mesh)
{
	bool ret = false;
	//Load a buffer to access the data of the .mesh
	char* buffer = nullptr;
	App->file_system->Load(full_path, &buffer);

	if (mesh == nullptr)
	{
		LOG("[WARNING] Tried to import a .mesh file into a nullptr mesh, a mesh will be created in memory");
		mesh = new Mesh;
	}

	//------------------- Assign data from buffer  -------------------//
	char* cursor = (char*)buffer;
	// amount of indices / vertices / colors / normals / texture_coords
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);

	mesh->num_index = ranges[0];
	mesh->num_vertex = ranges[1];
	mesh->num_normals = ranges[2];
	mesh->num_tex_coords = ranges[3];

	cursor += bytes;																		//																//
	bytes = sizeof(uint) * mesh->num_index;									//					Copy Indices 						//
	mesh->index = new uint[mesh->num_index];
	memcpy(mesh->index, cursor, bytes);										//																//

	cursor += bytes;																		//																//
	bytes = sizeof(float) * mesh->num_vertex * 3;						//					Copy Vertices   					//
	mesh->vertex = new float3[mesh->num_vertex];
	memcpy(mesh->vertex, cursor, bytes);										//																//

	cursor += bytes;																		//																//
	bytes = sizeof(float) * mesh->num_normals * 3;						//					Copy Normals Vector   			//
	mesh->normals_vector = new float3[mesh->num_normals];
	memcpy(mesh->normals_vector, cursor, bytes);						//																//

	cursor += bytes;																		//																//
	bytes = sizeof(float) * mesh->num_normals * 3;						//					Copy Normals Faces  			//
	mesh->normals_faces = new float3[mesh->num_normals];		//																//
	memcpy(mesh->normals_faces, cursor, bytes);							//																//

	cursor += bytes;																							//																					//
	bytes = sizeof(float) * mesh->num_normals * 3;											//					Copy Normals Faces vectors   					//
	mesh->normals_faces_vector = new float3[mesh->num_normals];				//																					//
	memcpy(mesh->normals_faces_vector, cursor, bytes);								//																					//

	cursor += bytes;																							//																					//
	bytes = sizeof(float) * mesh->num_tex_coords * 2;										//					Copy Texture Coordinates						//
	mesh->tex_coords = new float[mesh->num_tex_coords * 2];						//																					//
	memcpy(mesh->tex_coords, cursor, bytes);													//																					//

	return false;
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

	transform->position = pos_rot_scale[0];										//					Assign transform position					//
	transform->eulerRotation = pos_rot_scale[1];								//						"			"		  rotation					    //
	transform->scale = pos_rot_scale[2];											//						"		    "           scale					    //

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

bool Importer::ImportModel(const char * path, const char * file)
{
	bool ret = false;

	std::string output_file = path;
	output_file += file;

	//Load a buffer to access the data of the .mesh
	char* buffer = nullptr;
	App->file_system->Load(output_file.data(), &buffer);
	char* cursor = (char*)buffer;
	
	//Read the number of gameobjects present in the model
	uint num_gos;

	uint bytes = sizeof(num_gos);
	memcpy(&num_gos, cursor, bytes);
	cursor += bytes;

	std::vector<GameObject*> gos;

	for (int i = 0; i < num_gos; ++i)
	{
		//------------------- Assign data from buffer to each game object  -------------------//
		GameObject* go = new GameObject;

		bytes = sizeof(uint32_t);
		memcpy(&go->UID, cursor, bytes);

		cursor += bytes;
		bytes = sizeof(bool);
		memcpy(&go->active, cursor, bytes);

		cursor += bytes;
		bytes = sizeof(bool);
		memcpy(&go->staticObj, cursor, bytes);

		cursor += bytes;
		bytes = sizeof(std::string);
		memcpy(&go->name, cursor, bytes);
		go->name._Myproxy() = nullptr;  // The string tries to load a proxy, we must set it to nullptr or on delete std will try to delete out of bounds memory

		//Load Components
		//std::string component_paths[2];
		//bytes = sizeof(std::string)*2;
		//memcpy(component_paths,cursor,bytes);

		//Load Mesh Component
		/*if (component_paths[0].data() != "no_component")
		{
			Mesh* m = nullptr;
			Import(component_paths[0].data(), m);
			App->renderer3D->GenerateVertexBuffer(m->id_vertex, m->num_vertex, m->vertex);
			App->renderer3D->GenerateIndexBuffer(m->id_index, m->num_index, m->index);
			App->renderer3D->GenerateVertexBuffer(m->id_tex_coords, m->num_tex_coords * 2, m->tex_coords);

			ComponentMesh* mesh_comp = (ComponentMesh*)go->CreateComponent(COMPONENT_TYPE::MESH);
			mesh_comp->mesh = m;
		}*/

		

		gos.push_back(go);
		cursor += bytes; //prepare cursor to read next gameobject
	}

	//For now parent all game objects to rot
	for (int i = 0; i < num_gos; ++i)
	GOFunctions::ReParentGameObject(gos[i],App->scene_intro->root);

	return false;
}

//Export a Mesh as our own file format .mesh
bool Importer::Export(const char * path, std::string & output_file,const  Mesh* mesh, const char* filename)
{
	bool ret = false;

	//Amount of / indices / vertices / normals / tex_coords
	uint ranges[4] = {mesh->num_index, mesh->num_vertex, mesh->num_normals, mesh->num_tex_coords};
	uint size = 0; // This will be the size in bytes that we will need to allocate

	size += sizeof(ranges) + sizeof(uint) * mesh->num_index + sizeof(float) * mesh->num_vertex * 3; // Size of alloc for ranges, indices, and vertices
	size += sizeof(float) * mesh->num_normals * 3 * 3; //Size of alloc of all 3 normals data containers
	size += sizeof(float) * mesh->num_normals * 2; // Size of alloc for Texture coordinates


	char* data = new char[size]; // Allocate buffer 
	char* cursor = data; // Cursor at start

	//-------------------  Store data  -------------------//
	uint bytes = sizeof(ranges); // First store ranges Bytes is the ammount of bytes to copy or to move the cursor
	memcpy(cursor, ranges, bytes);

	// Store indices
	cursor += bytes; 
	bytes = sizeof(uint) * mesh->num_index;
	memcpy(cursor, mesh->index, bytes);

	// Store Vertices
	cursor += bytes; 
	bytes = sizeof(float) * mesh->num_vertex * 3;
	memcpy(cursor,mesh->vertex,bytes);

	//Store the 3 normals containers
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_normals * 3;
	memcpy(cursor,mesh->normals_vector,bytes);

	cursor += bytes;
	bytes = sizeof(float) * mesh->num_normals * 3;
	memcpy(cursor, mesh->normals_faces, bytes);

	cursor += bytes;
	bytes = sizeof(float) * mesh->num_normals * 3;
	memcpy(cursor, mesh->normals_faces_vector, bytes);

	//Store texture coordinates
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_tex_coords * 2;
	memcpy(cursor, mesh->tex_coords, bytes);

	//Save the new .mesh to disk
	ret = App->file_system->SaveUnique(output_file, data, size, path,filename,"mesh");

	if (ret == true)
	{
		LOG("Succesfully exported %s to %s as a .mesh", output_file, path);
	}
	else
		LOG("Failed to export %s to %s as a .mesh", output_file, path);

	return ret;
}

bool Importer::Export(const char * path, std::string & output_file, const ComponentTransform * transform, const char * filename)
{
	bool ret = false;

	uint size = 0; // This will be the size in bytes that we will need to allocate
	size += sizeof(float3) * 3  + sizeof(float4x4) * 2;

	char* data = new char[size];
	char* cursor = data;

	float3 pos_rot_scale[3] = {transform->position, transform->eulerRotation, transform->scale};

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

bool Importer::Export(const char * path, std::string & output_file, const GameObject * go, const char * filename)
{
	bool ret = false;

	//Calculate the total size depending on number of gameobject

	std::vector<const GameObject*> gos;
	GOFunctions::FillArrayWithChildren(gos, go, true); // fill an array with go and all its children gameobjects 
	uint num_gos = gos.size();
	
	uint size = 0; // This will be the size in bytes that we will need to allocate
	size += sizeof(num_gos) + sizeof(GameObject) * num_gos;
	size -= sizeof(go->children) + sizeof(go->components) * num_gos;  //We subtract the size of the thing swe won't be using (we can't pass )
	//size += sizeof(std::string) * 2 * num_gos; // size for paths of components
	char* data = new char[size];
	char* cursor = data;
	uint bytes = 0;

	//Save the number of gameobjects so when reading we can know the number of iterations we will need to do
	bytes = sizeof(num_gos);
	memcpy(cursor,&num_gos,bytes);
	cursor += bytes;

	const GameObject* go_save = nullptr;
	//---------------------- Store the data --------------------------//
	for (int i = 0; i < num_gos; ++i)
	{
		go_save = gos[i];
		bytes = sizeof(go_save->UID);
		uint32_t t = go_save->UID;
		memcpy(cursor, &t, bytes);

		cursor += bytes;
		bytes = sizeof(bool);
		memcpy(cursor, &go_save->active, bytes); //store active

		cursor += bytes;
		bytes = sizeof(bool);
		memcpy(cursor, &go_save->staticObj, bytes); //store static

		cursor += bytes;
		bytes = sizeof(go_save->name); //Store name
		memcpy(cursor, &go_save->name, bytes);

		//Save Components
		/*std::string output_file_mesh;
		ComponentMesh* m = (ComponentMesh*)go_save->GetComponent(COMPONENT_TYPE::MESH);
		if (m != nullptr)
			Export(LIBRARY_MESH_FOLDER, output_file_mesh, m->mesh);
		else
			output_file_mesh = "no_component";

		std::string output_file_trans;
		ComponentTransform* trans = (ComponentTransform*)go_save->GetComponent(COMPONENT_TYPE::TRANSFORM);
		if (trans != nullptr)
			Export(LIBRARY_TRANSFORMATIONS_FOLDER, output_file_trans, trans);
		else
			output_file_trans = "no_component";*/


		//Now we save the paths to the components
		/*output_file_mesh._Myproxy() = nullptr;
		output_file_trans._Myproxy() = nullptr;
		std::string paths[2] = { output_file_mesh, output_file_trans };
		cursor += bytes;
		bytes = sizeof(std::string)* 2;
		memcpy(cursor,paths,bytes);*/

		cursor += bytes; //This way we can loop this whole process for multiple gameobjects
	}

	//Save the new .model to disk
	ret = App->file_system->SaveUnique(output_file, data, size, path, filename, "model");

	if (ret == true)
	{
		LOG("Succesfully exported %s to %s as a .model", output_file, path);
	}
	else
		LOG("Failed to export %s to %s as a .model", output_file, path);


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
