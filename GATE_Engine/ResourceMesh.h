#ifndef __RESOURCE_MESH_H__
#define __RESOURCE_MESH_H__

#include "Resource.h"
#include "JsonLoader.h"
#include "libs/MathGeoLib/include/Geometry/AABB.h"

struct aiMesh;

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint32 uid);
	~ResourceMesh();

public:
	bool LoadInMemory() override; 
	void Save(json& config) const override; 
	void Load(const json& config) override;

	/*void LoadVertices(const aiMesh* loaded_mesh);
	void LoadIndices(const aiMesh* loaded_mesh);
	void LoadNormals(const aiMesh* loaded_mesh);
	void LoadTexCoords(const aiMesh* loaded_mesh);
	void ChangeNormalsLength(float new_length);
	void LoadMeshBounds();*/

	AABB GetBounds() { return bounds; }
	float3 GetSize() { return size; }

public:
	std::string path;
	std::string filename;

	uint id_index = 0; // index in VRAM 
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM 
	uint num_vertex = 0;
	float3* vertex = nullptr;

	uint id_normals = 0; // index in VRAM
	uint num_normals = 0;
	float3* normals_vector = nullptr; // Store the Vectors of the normals
	float3* normals_faces = nullptr; // Since on resize vertex positions will change we store the position
	float3* normals_faces_vector = nullptr;

	uint id_tex_coords = 0; // index in VRAM
	uint num_tex_coords = 0;
	float* tex_coords = nullptr;

	AABB bounds;	// In local space of the imported FBX
	float3 size = { 0.0f, 0.0f, 0.0f };	// Absolute distance between bound max/min points

	float normals_length = 1.0f;
	float normals_last_length = 1.0f;

	unsigned long int num_polys = 0;
};
#endif // !__RESOURCE_MESH_H__

