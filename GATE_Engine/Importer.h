#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"
#include <string>

class Mesh;
class ComponentTransform;

class Importer { 
public:
	Importer();
	~Importer();
public:
	bool Import(const char* file, const char* path, std::string& output_file, Mesh* mesh);
	bool Import(const char* file, const char* path, std::string& output_file, ComponentTransform* transform);

	bool ImportToMesh(const void* buffer, uint size, std::string& output_file, Mesh* mesh);
	bool Export(const char* path, std::string& output_file, const Mesh* mesh, const char* filename);
	bool Export(const char* path, std::string& output_file, const ComponentTransform* transform, const char* filename);
};

#endif