#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

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
		CheckDirectoryUpdate(assets_dir);
		CheckFilesUpdate(assets_dir);
		timer.Start();
	}
	return update_status::UPDATE_CONTINUE;
}

uint32 ModuleResources::Find(const char * file_in_assets) const
{
	return 0;
}

uint32 ModuleResources::ImportFile(const char * new_file_in_assets, Resource::Type type, bool force)
{
	uint32 ret = 0; bool import_ok = false; std::string written_file;

	switch (type)
	{
	case Resource::TEXTURE: import_ok = App->texture_loader->importer.ImportableResource(new_file_in_assets); break;
	}
	import_ok = true; //TODO: DIdac remove this hardcode which serves to test texture creation
	if (import_ok == true)
	{ // If import was successful, create a new resource 
		Resource* res = nullptr;
		//Resource* res = CreateNewResource(type);
		switch (type)
		{
		case Resource::TEXTURE:	res = App->texture_loader->LoadTextureFile(new_file_in_assets);
		}
		//res->file = new_file_in_assets;
		//res->exported_file = written_file;
		ret = res->GetUID();
	}

	return ret;
}

uint32 ModuleResources::GenerateNewUID()
{
	return App->rng.RandInt<uint32>();
}

const Resource * ModuleResources::Get(uint32 uid) const
{
	std::map<uint32, Resource*>::const_iterator it = resources.find(uid); 
	if (it != resources.end()) 
		return it->second; 
	return nullptr;
}

Resource * ModuleResources::Get(uint32 uid)
{
	std::map<uint32, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return it->second;
	return nullptr;
}

Resource * ModuleResources::CreateNewResource(Resource::Type type, uint32 force_uid)
{
	Resource* ret = nullptr; 
	uint32 uid;
	if (force_uid == 0)
		uid = GenerateNewUID();
	else
		uid = force_uid;

	switch (type) 
	{ 
		case Resource::MESH:      ret = (Resource*) new ResourceMesh(uid);      break; 
		case Resource::TEXTURE: ret = (Resource*) new ResourceTexture(uid);  break;
	}
	
	if (ret != nullptr) 
		resources[uid] = ret;
return ret;
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
		new_dir.dir_name = discovered_dirs[i];
		new_dir.dir_path = abs_dir.dir_path + discovered_dirs[i] + "/";
		abs_dir.sub_dirs.push_back(new_dir);
	}

	for (int i = 0; i < abs_dir.sub_dirs.size(); ++i)
		InitPopulateAssetsDir(abs_dir.sub_dirs[i]);
}

void ModuleResources::CheckDirectoryUpdate(AbstractDir & abs_dir)
{
	std::vector<std::string> discovered_files, discovered_dirs;

	App->file_system->DiscoverFiles(abs_dir.dir_path.data(), discovered_files, discovered_dirs);

	int size_diff = abs_dir.sub_dirs.size() - discovered_dirs.size();
	if (size_diff == 0)	//If no subdirectory was added or removed
	{
		//Check if the name changed
	}
	else if (size_diff != 0)	// If sub directory added or removed
	{
		if (size_diff > 0)	// A directory was removed
		{
			//Check on the known directories which ones were deleted
			for (int i = 0; i < abs_dir.sub_dirs.size(); ++i)
			{
				if (!App->file_system->Exists(abs_dir.sub_dirs[i].dir_path.data()))
				{
					LOG("[Info]: Directory %s and all its dependencies were removed", abs_dir.sub_dirs[i].dir_path.data());
					abs_dir.sub_dirs.erase(abs_dir.sub_dirs.begin() + i);
				}
			}
		}
		else //A directory was added
		{
			// We will remove from the discovered vector all elements that are already known, it's not the most optimal way to do this, but at least we are doing some optimization
			// instead of just comparing all elements of a against elements of b each iteration
			for (int i = 0; i < abs_dir.sub_dirs.size(); ++i)
			{
				for (int j = 0; j < discovered_dirs.size(); ++j)
				{
					if (abs_dir.sub_dirs[i].dir_name == discovered_dirs[j])
					{
						discovered_dirs.erase(discovered_dirs.begin() + j);
						break;
					}
				}
			}

			//We have discarded all common elements in discovered directories, so the array contains only the different dirs
			for (int i = 0; i < discovered_dirs.size(); ++i)
			{
				AbstractDir new_dir;
				new_dir.dir_name = discovered_dirs[i];
				new_dir.dir_path = abs_dir.dir_path + discovered_dirs[i] + "/";
				abs_dir.sub_dirs.push_back(new_dir);
				LOG("[Info]: Directory %s was created", new_dir.dir_path.data());
			}
		}
	}
	// Call Recursive
	for (int i = 0; i < abs_dir.sub_dirs.size(); ++i)
		CheckDirectoryUpdate(abs_dir.sub_dirs[i]);
}

void ModuleResources::CheckFilesUpdate(AbstractDir & abs_dir)
{
	std::vector<std::string> discovered_files, discovered_dirs;

	App->file_system->DiscoverFiles(abs_dir.dir_path.data(), discovered_files, discovered_dirs);

	int size_diff = abs_dir.files.size() - discovered_files.size();
	if (size_diff == 0)	//If no subdirectory was added or removed
	{
		//Check if the name changed
	}
	else if (size_diff != 0)	// If sub directory added or removed
	{
		if (size_diff > 0)	// A directory was removed
		{
			std::string file_path;
			//Check on the known directories which ones were deleted
			for (int i = 0; i < abs_dir.files.size(); ++i)
			{
				file_path = abs_dir.dir_path + abs_dir.files[i];
				if (!App->file_system->Exists(file_path.data()))
				{
					LOG("[Info]: File %s was removed from %s", abs_dir.files[i].data(), abs_dir.dir_path.data());
					abs_dir.files.erase(abs_dir.files.begin() + i);
				}
			}
		}
		else //A directory was added
		{
			// We will remove from the discovered vector all elements that are already known, it's not the most optimal way to do this, but at least we are doing some optimization
			// instead of just comparing all elements of a against elements of b each iteration
			for (int i = 0; i < abs_dir.files.size(); ++i)
			{
				for (int j = 0; j < discovered_files.size(); ++j)
				{
					if (abs_dir.files[i] == discovered_files[j])
					{
						discovered_files.erase(discovered_files.begin() + j);
						break;
					}
				}
			}

			//We have discarded all common elements in discovered directories, so the array contains only the different dirs
			for (int i = 0; i < discovered_files.size(); ++i)
			{
				abs_dir.files.push_back(discovered_files[i]);
				LOG("[Info]: File %s was added at %s", discovered_files[i].data(), abs_dir.dir_path.data());
			}
		}
	}

	// Call Recursive
	for (int i = 0; i < abs_dir.sub_dirs.size(); ++i)
		CheckFilesUpdate(abs_dir.sub_dirs[i]);
}
