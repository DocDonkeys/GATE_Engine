#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "Resource.h"

ModuleResources::ModuleResources(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{

}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init()
{
	assets_dir.dir_path = ASSETS_FOLDER;
	InitPopulateAssetsDir(assets_dir);
	timer.Start();
	return false;
}

update_status ModuleResources::PreUpdate(float dt)
{
	float time_passed = timer.ReadSec(); //Time passed after update of data

	if (time_passed >= 1.5f)
	{
		//std::vector<std::string> discovered_files, discovered_dirs;

		//App->file_system->DiscoverFiles(assets_dir.dir_path.data(),discovered_files, discovered_dirs);

		//int size_diff = assets_dir.sub_dirs.size() - discovered_dirs.size();
		//
		//if (size_diff == 0)	//If no subdirectory was added or removed
		//{
		//	//Check if the name changed
		//}
		//else if (size_diff != 0)	// If sub directory added or removed
		//{
		//	// Substitute known dirs by the ones retrieved
		//	assets_dir.sub_dirs.clear();

		//}


		timer.Start();
	}
	return update_status::UPDATE_CONTINUE;
}

uint32 ModuleResources::Find(const char * file_in_assets) const
{
	return 0;
}

uint32 ModuleResources::ImportFile(const char * new_file_in_assets, bool force)
{
	return 0;
}

uint32 ModuleResources::GenerateNewUID()
{
	return 0;
}

const Resource * ModuleResources::Get(uint32 uid) const
{
	return nullptr;
}

Resource * ModuleResources::Get(uint32 uid)
{
	return nullptr;
}

Resource * ModuleResources::CreateNewResource(Resource::Type type, uint32 force_uid)
{
	return nullptr;
}

void ModuleResources::InitPopulateAssetsDir(AbstractDir &abs_dir)
{
	LOG("Loading Assets folder hierarchy.");
	std::vector<std::string> discovered_files, discovered_dirs;

	App->file_system->DiscoverFiles(abs_dir.dir_path.data(), discovered_files, discovered_dirs);

	for (int i = 0; i < discovered_files.size(); ++i)
	{
		abs_dir.files.push_back(discovered_files[i]);
	}

	for (int i = 0; i < discovered_dirs.size(); ++i)
	{
		AbstractDir new_dir;
		new_dir.dir_path = abs_dir.dir_path + discovered_dirs[i] + "/";
		abs_dir.sub_dirs.push_back(new_dir);
	}

	for (int i = 0; i < abs_dir.sub_dirs.size(); ++i)
		InitPopulateAssetsDir(abs_dir.sub_dirs[i]);
}
