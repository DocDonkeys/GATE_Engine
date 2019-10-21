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
	void LoadMaterials(const aiScene* scene, const aiMesh* loaded_mesh, const std::string& absolute_path);

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

	uint id_texture = 0; // Texture index in VRAM
};
#endif
