#ifndef __MESH_H__
#define __MESH_H__

#include "Globals.h"
#include "GeometryLoader.h"
struct aiMesh;

class Mesh
{
public:
	Mesh();
	~Mesh();
public: // Calls
	void LoadVertices(const aiMesh* loaded_mesh);
	void LoadIndices(const aiMesh* loaded_mesh);
	void LoadNormals(const aiMesh* loaded_mesh);
	void LoadTexCoords(const aiMesh* loaded_mesh);
	void LoadMeshSizeData();

public: // Vars
	uint id_index = 0; // index in VRAM 
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM 
	uint num_vertex = 0;
	float3* vertex = nullptr;

	uint id_normals = 0; // index in VRAM
	uint num_normals = 0;
	uint num_faces = 0;
	float3* normals_vector = nullptr; // Store the Vectors of the normals
	float3* normals_faces = nullptr; // Since on resize vertex positions will change we store the position
	float3* normals_faces_vector = nullptr;

	uint id_tex_coords = 0; // index in VRAM
	uint num_tex_coords = 0;
	float* tex_coords = nullptr;

	struct size_data {	// Mesh size data
		float max = -10.0f;
		float min = 10.0f;	// Using these initializing values we make sure that both max and min are substituted for vertex pos values, as max is too low and min too high for the usual vertex values
		float width = 0.0f;
	};
	size_data size_x, size_y, size_z;
};
#endif
