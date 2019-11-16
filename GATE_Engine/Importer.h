#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"
#include <string>

class Mesh;
class ComponentTransform;
class GameObject;

class Importer { 
public:
	Importer();
	~Importer();
public:
	bool Import(const char* file, const char* path, std::string& output_file, Mesh* mesh);
	bool Import(const char* full_path, Mesh* mesh);
	bool Import(const char* file, const char* path, std::string& output_file, ComponentTransform* transform);
	bool ImportModel(const char* path, const char* file);

	bool Export(const char* path, std::string& output_file, const Mesh* mesh, const char* filename = "");
	bool Export(const char* path, std::string& output_file, const ComponentTransform* transform, const char* filename = "");
	bool Export(const char* path, std::string& output_file, const GameObject* go, const char* filename);

	uint TotalGOsNum(const GameObject* go, uint& num_of_gos);
};

#endif