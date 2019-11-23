#include "ImporterMesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleRenderer3D.h"
#include "ModuleResources.h"
#include "ResourceMesh.h"

ImporterMesh::ImporterMesh()
{
}

ImporterMesh::~ImporterMesh()
{
}

bool ImporterMesh::Export(const char * path, std::string & output_file, const ImportExportData * ie_data, const char * filename)
{
	bool ret = false;
	IEMeshData* ie_mdata= (IEMeshData*)ie_data;
	ResourceMesh* mesh = ie_mdata->mesh;

	//Amount of / indices / vertices / normals / tex_coords
	uint ranges[4] = { mesh->num_index, mesh->num_vertex, mesh->num_normals, mesh->num_tex_coords };
	uint size = 0; // This will be the size in bytes that we will need to allocate

	size += sizeof(ranges) + sizeof(uint) * mesh->num_index + sizeof(float) * mesh->num_vertex * 3; // Size of alloc for ranges, indices, and vertices
	size += sizeof(float) * mesh->num_normals * 3 * 3; //Size of alloc of all 3 normals data containers
	size += sizeof(float) * mesh->num_normals * 2; // Size of alloc for Texture coordinates
	size += sizeof(AABB) + sizeof(float3); //Size of alloc of AABB & size


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
	memcpy(cursor, mesh->vertex, bytes);

	//Store the 3 normals containers
	cursor += bytes;
	bytes = sizeof(float) * mesh->num_normals * 3;
	memcpy(cursor, mesh->normals_vector, bytes);

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

	//Store AABB
	cursor += bytes;
	bytes = sizeof(AABB);
	memcpy(cursor,&mesh->bounds,bytes);

	//Store size
	cursor += bytes;
	bytes = sizeof(float3);
	memcpy(cursor, &mesh->size, bytes);

	//Save the new .mesh to disk
	ret = App->file_system->SaveUnique(output_file, data, size, path, filename, "mesh");

	if (ret == true)
	{
		LOG("Succesfully exported %s", output_file);
	}
	else
		LOG("Failed to export %s to %s as a .mesh", output_file, path);
	
	return ret;
}

bool ImporterMesh::Load(const char * full_path, ResourceMesh* mesh)
{
	bool ret = false;
	//Load a buffer to access the data of the .mesh
	char* buffer = nullptr;
	App->file_system->Load(full_path, &buffer);

	if (mesh == nullptr)
	{
		LOG("[WARNING] Tried to import a .mesh file into a nullptr mesh, a mesh will be created in memory");
		ResourceMesh* mesh = (ResourceMesh*)App->resources->CreateNewResource(Resource::MESH);
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

	cursor += bytes;																							//																					//
	bytes = sizeof(AABB);																					//					Copy AABB												//
	memcpy(&mesh->bounds, cursor, bytes);														//																					//

	cursor += bytes;																							//																					//
	bytes = sizeof(float3);																					//							Copy Size											//
	memcpy(&mesh->size,cursor,bytes);																//																					//

	//Alloc buffers for rendering
	GenMeshBuffers(mesh);
	return false;
}

bool ImporterMesh::CreateMeta(const char * original_file_full_path, IEMeshData * ie_data)
{
	bool ret = false;
	json file; //File to save
	std::string path, filename, extension;
	App->file_system->SplitFilePath(original_file_full_path, &path, &filename, &extension);

	//Data saving
	file["Path original"] = original_file_full_path;
	file["UID"] = ie_data->mesh->GetUID();
	file["Num:Indices"] = ie_data->mesh->num_index;
	file["Num:Vertex"] = ie_data->mesh->num_vertex;
	file["Num:Poly"] = ie_data->mesh->num_polys;

	//Convert to buffer
	std::string data = App->jLoad.JsonToString(file);
	char* buffer = (char*)data.data();
	std::string output;

	//Save File
	ret = App->file_system->SaveUnique(output, buffer, data.length(), path.data(), filename.data(), "meta");
	return ret;
}

bool ImporterMesh::GenMeshBuffers(ResourceMesh* mesh)
{
	App->renderer3D->GenerateVertexBuffer(mesh->id_vertex, mesh->num_vertex, mesh->vertex);
	if (mesh->index != nullptr)
		App->renderer3D->GenerateIndexBuffer(mesh->id_index, mesh->num_index, mesh->index);

	App->renderer3D->GenerateVertexBuffer(mesh->id_tex_coords, mesh->num_tex_coords * 2, mesh->tex_coords);
	return false;
}
