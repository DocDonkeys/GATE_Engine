#ifndef __GEOMETRYLOADER_H__
#define __GEOMETRYLOADER_H__

#include "Globals.h"
#include "Module.h"
#include <vector>

#include "libs/MathGeoLib/include/Math/float3.h"

#include "libs/Assimp/include/cimport.h"

struct par_shapes_mesh_s;

enum class PRIMITIVE
{
	PLANE = 0,
	CUBE,
	SPHERE,
	HEMISPHERE,
	CYLINDER,
	CONE,
	TORUS
};

struct Mesh_Data //Struct to hold info for meshes
{
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

class GeometryLoader : public Module
{
public:
	GeometryLoader(Application* app, const char* name = "null", bool start_enabled = true);
	~GeometryLoader();

public:
	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	bool Load3DFile(const char* full_path); // Load a 3D file such as an FBX, OBJ etc.
	void LoadPrimitiveShape(par_shapes_mesh_s* p_mesh); // Load a generated primitive (par_shape) into a mesh (Mesh_Data)

	//Generate a primitive, for CUBE (slices,stacks,radius) will be ignored, for anything else except Torus (radius) will be ignored. Remember radius between 0 & 1.0f
	void CreatePrimitive(PRIMITIVE p, int slices = 0, int stacks = 0, float radius = 0.f);

public: // Vars
	aiLogStream stream;
	std::vector<Mesh_Data*> meshes;
};


#endif //__GEOMETRYLOADER_H__