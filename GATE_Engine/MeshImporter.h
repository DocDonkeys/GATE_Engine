#ifndef __MESHIMPORTER_H__
#define __MESHIMPORTER_H__

#include "Globals.h"
#include <string>

class Mesh;

class MeshImporter { 
public:
	MeshImporter();
	~MeshImporter();
public:
	bool Import(const char* file, const char* path, std::string& output_file, Mesh* mesh); 
	bool ImportToMesh(const void* buffer, uint size, std::string& output_file, Mesh* mesh);
	bool Export(const char* path, std::string& output_file, const Mesh* mesh, const char* filename);
};

#endif