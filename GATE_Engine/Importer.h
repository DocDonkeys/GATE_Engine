#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"
#include <string>
#include "JsonLoader.h"

class ComponentTransform;
class GameObject;

struct ImportExportData 
{
	std::string meta_path;  //Path that will be saved into the meta file
}; // We will need to overload some functions with diferent parameters. Inheriting this struct will allow us to do so

class Importer { 
public:
	Importer();
	~Importer();
public:
	bool Import(const char* file, const char* path, std::string& output_file, ComponentTransform* transform);

	bool Export(const char* path, std::string& output_file, const ComponentTransform* transform, const char* filename = "");

	virtual bool Export(const char* path, std::string& output_file, const ImportExportData* ie_data, const char* filename = "");
	virtual bool Load(const char* full_path);
	virtual bool CreateMeta(const char* original_file_full_path, ImportExportData* ie_data);
	virtual bool LoadMeta(const char* full_path);

	bool ImportableResource(const char* full_path);
	uint TotalGOsNum(const GameObject* go, uint& num_of_gos);

public:
	ImportExportData ie_data;
};

#endif