#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneIntro.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "TextureLoader.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "libs/SDL/include/SDL_assert.h"

#include "MemLeaks.h"

ModuleResources::ModuleResources(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{

}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::CleanUp()
{
	return false;
}

bool ModuleResources::Init()
{
	assets_dir.dir_path = ASSETS_FOLDER;
	InitPopulateAssetsDir(assets_dir);
	timer.Start();

	//Fill extensions IMPROVE: load from config file
	extension_3D_file.push_back("fbx");
	extension_3D_file.push_back("FBX");
	extension_3D_file.push_back("obj");
	extension_3D_file.push_back("OBJ");

	extension_texture.push_back("png");
	extension_texture.push_back("PNG");
	extension_texture.push_back("jpg");
	extension_texture.push_back("dds");
	extension_texture.push_back("DDS");
	extension_texture.push_back("tga");
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

uint32 ModuleResources::ImportFile(const char * full_path)
{
	uint uid = 0;
	std::string file, extension, path;
	path = full_path;
	App->file_system->NormalizePath(path);
	App->file_system->SplitFilePath(full_path,nullptr,&file,&extension);

	Resource::Type type = ResourceTypeByPath(extension.data());
	path = file;
	if (type == Resource::TEXTURE) // Since textures will be exported as dds into assets default textures folder we need to change extesnion to .dds before making any checks
	{
		path += ".meta";
		path = ASSETS_DEFAULT_TEXTURES + path;
	}
	else if (type == Resource::MODEL)
	{
		path += ".meta";
		path = ASSETS_DEFAULT_MESHES + path;
	}
	
	bool has_meta = false;  //Debugging for the big crash in .exe when loading a fbx
	has_meta = App->file_system->Exists(path.data());
	path = App->SubtractString(path,".",true,true);		//We take .meta out of the path since we are not checking anymore

	std::string  meta_info_path, meta_file_path;
	ImportExportData metadata;
	GameObject* new_model = nullptr;
	ResourceTexture* tex = nullptr;
	switch (type)
	{
	case Resource::UNKNOWN:
		break;
	case Resource::MESH:
		break;
	case Resource::TEXTURE:
		if (has_meta)
		{
			meta_file_path = path;
			path += ".meta";
			SDL_assert_release(has_meta == true, path);
			tex = App->texture_loader->importer.LoadMeta(path.data(),true);
			
			//tex = App->texture_loader->LoadTextureFile(full_path);
			uid = tex->GetUID();
		}
		else
		{
			tex = App->texture_loader->importer.LoadTexture(full_path,true);
			//tex = App->texture_loader->LoadTextureFile(full_path, true);
			meta_info_path = "_t";
			meta_info_path += std::to_string(tex->GetUID());
			meta_info_path += ".dds";
			meta_info_path = LIBRARY_TEXTURES_FOLDER + meta_info_path;

			metadata.meta_path = meta_info_path;
			meta_file_path = path;
			App->texture_loader->importer.CreateMeta(meta_file_path.data(), &metadata);
			uid = tex->GetUID();
		}
		break;
	case Resource::SCENE:
		break;
	case Resource::MODEL:
		if (has_meta)
		{
			path += ".meta";
			App->scene_intro->scene_ie.LoadMeta(path.data(),true);
		}
		else
		{
			new_model = App->geometry_loader->Load3DFile(full_path);
			meta_info_path = App->scene_intro->scene_ie.SaveScene(new_model, new_model->name, FileType::MODEL);

			//We create the string to duplicate the fbx
			meta_file_path = ASSETS_DEFAULT_MESHES + file;
			App->file_system->CopyFromOutsideFS(full_path, meta_file_path.data());
			metadata.meta_path = meta_info_path;
			App->scene_intro->scene_ie.CreateMeta(meta_file_path.data(), &metadata);
		}
		
		break;
	default:
		break;
	}

	return uid;
}

uint32 ModuleResources::ImportInternalFile(const char * new_file_in_assets, Resource::Type type, bool force)
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

Resource::Type ModuleResources::ResourceTypeByPath(const std::string extension)
{
	if (extension.data() == "mesh")
		return Resource::MESH;

	if (extension.data() == "dds")
		return Resource::TEXTURE;

	if (extension.data() == "scene")
		return Resource::SCENE;

	for (int i = 0; i < extension_3D_file.size(); ++i)
		if (extension == extension_3D_file[i])
			return Resource::MODEL;

	for (int i = 0; i < extension_texture.size(); ++i)
		if (extension == extension_texture[i])
			return Resource::TEXTURE;

	return Resource::UNKNOWN;
}

uint32 ModuleResources::GetUIDFromPath(const char * full_path, Resource::Type type)
{
	uint32 ret = 0;
	std::string filename;
	App->file_system->SplitFilePath(full_path,nullptr,&filename,nullptr);
	filename = App->SubtractString(filename,".",true,true);

	switch (type)
	{
	case Resource::MESH:
		filename = App->SubtractString(filename, "m", false, false);
		ret = std::stoul(filename);
		break;
	case Resource::TEXTURE:
		filename = App->SubtractString(filename, "t", false, false);
		ret = std::stoul(filename);
		break;
	default:
		break;
	}

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
