#include "MeshImporter.h"
#include "Mesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"

MeshImporter::MeshImporter()
{
}

MeshImporter::~MeshImporter()
{
}

bool MeshImporter::Import(const char * file, const char * path, std::string & output_file, Mesh* mesh)
{
	bool ret = false;
	output_file = path;
	output_file += file;

	//Load a buffer to access the data of the .mesh
	char* buffer = nullptr;
	App->file_system->Load(output_file.data(), &buffer);

	if (mesh == nullptr)
	{
		LOG("[WARNING] Tried to import a .mesh file into a nullptr mesh, a mesh in memory will be created");
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

bool MeshImporter::ImportToMesh(const void * buffer, uint size, std::string & output_file, Mesh* mesh)
{
	bool ret = false;
	
	
	

	return ret;
}

//Export a Mesh as our own file format .mesh
bool MeshImporter::Export(const char * path, std::string & output_file,const  Mesh* mesh, const char* filename)
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
