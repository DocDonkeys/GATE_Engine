#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Globals.h"
#include "Module.h"
#include "Resource.h"
#include "Timer.h"
#include <string>
#include <map>

class Application;

struct AbstractDir
{
	std::string dir_path;
	std::string dir_name;
	std::vector<AbstractDir> sub_dirs;
	std::vector<std::string> files;
};

class ModuleResources : public Module
{
public:
	ModuleResources (Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleResources();

	bool Init();
	update_status PreUpdate(float dt);

public:
	uint32 Find(const char* file_in_assets) const;
	uint32 ImportFile(const char* new_file_in_assets, bool force = false);
	uint32 GenerateNewUID();
	const Resource* Get(uint32 uid) const;
	Resource* Get(uint32 uid);
	Resource* CreateNewResource(Resource::Type type, uint32 force_uid = 0);

private:
	//This function initially fills the directories and files assets tree, with all data found inside the Assets folder
	void InitPopulateAssetsDir(AbstractDir &abs_dir);

	void CheckDirectoryUpdate(AbstractDir &abs_dir);

private: 
	std::map<uint32, Resource*> resources;

	//File monitoring
	Timer timer;
	std::vector<std::string> known_files;
	std::vector<std::string> known_dirs;
	AbstractDir assets_dir;
};
#endif // !__MODULE_RESOURCES_H__

