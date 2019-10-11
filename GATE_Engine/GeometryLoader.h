#ifndef __GEOMETRYLOADER_H__
#define __GEOMETRYLOADER_H__

#include "Globals.h"
#include "Module.h"
#include <vector>

#include "libs/Assimp/include/cimport.h"

struct Mesh_Data //Struct to hold info for meshes imported with assimp
{
	uint id_index = 0; // index in VRAM 
	uint num_index = 0; 
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM 
	uint num_vertex = 0; 
	float* vertex = nullptr;
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
	bool Load3DFile(const char* full_path);

public: // Vars
	aiLogStream stream;
	std::vector<Mesh_Data*> meshes;
};


#endif //__GEOMETRYLOADER_H__