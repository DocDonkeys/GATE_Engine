#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"
#include <string>

class ComponentTransform;
class GameObject;

struct ImportExportData {}; // We will need to overload some functions with diferent parameters. Inheriting this struct will allow us to do so

class Importer { 
public:
	Importer();
	~Importer();
public:
	bool Import(const char* file, const char* path, std::string& output_file, ComponentTransform* transform);
	bool ImportModel(const char* path, const char* file);

	bool Export(const char* path, std::string& output_file, const ComponentTransform* transform, const char* filename = "");
	bool Export(const char* path, std::string& output_file, const GameObject* go, const char* filename);

	virtual bool Export(const char* path, std::string& output_file, const ImportExportData* ie_data, const char* filename = "");
	virtual bool Load(const char* full_path);

	bool ImportableResource(const char* full_path);
	uint TotalGOsNum(const GameObject* go, uint& num_of_gos);
};

#endif