#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Globals.h"
#include "Module.h"
#include "Resource.h"
#include <string>
#include <map>

class Application;

class ModuleResources : public Module
{
public:
	ModuleResources (Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleResources();

public:
	uint32 Find(const char* file_in_assets) const;
	uint32 ImportFile(const char* new_file_in_assets, bool force = false);
	uint32 GenerateNewUID();
	const Resource* Get(uint32 uid) const;
	Resource* Get(uint32 uid);
	Resource* CreateNewResource(Resource::Type type, uint32 force_uid = 0);

	private: 
	std::map<uint32, Resource*> resources;
};
#endif // !__MODULE_RESOURCES_H__

