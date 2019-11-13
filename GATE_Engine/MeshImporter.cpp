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

bool MeshImporter::Import(const char * file, const char * path, std::string & output_file)
{
	return false;
}

bool MeshImporter::Import(const void * buffer, uint size, std::string & output_file)
{
	return false;
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
