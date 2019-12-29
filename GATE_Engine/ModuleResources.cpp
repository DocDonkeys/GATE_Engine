#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneIntro.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "TextureLoader.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceScript.h"
#include "ModuleScripting.h"
#include "libs/SDL/include/SDL_assert.h"

#include "ImporterScene.h"
#include "ImporterMaterial.h"

#include "MemLeaks.h"

ModuleResources::ModuleResources(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{

}

ModuleResources::~ModuleResources()
{
}

AbstractDir::~AbstractDir()
{
	//Delete all sub directories which will destroy all sub directories recursively (since delet will call the destructor)
	for (int i = 0; i < sub_dirs.size(); ++i)
	{
		RELEASE(sub_dirs[i]);
		sub_dirs.erase(sub_dirs.begin() + i);
	}
}

bool ModuleResources::CleanUp()
{
	RELEASE(ie_material);
	RELEASE(ie_scene);

	std::map<uint32, Resource*>::iterator it;

	for (it = resources.begin(); it != resources.end(); it++)
	{
		if ((*it).second != nullptr)
		{
			if((*it).second->GetType() == (Resource::Type::MESH || Resource::Type::MODEL ||
				Resource::Type::SCENE || Resource::Type::SCRIPT || Resource::Type::TEXTURE || Resource::Type::UNKNOWN))
			delete (*it).second;
		}
	}

	resources.clear();
	return false;
}

bool ModuleResources::Init()
{
	assets_dir = new AbstractDir;
	assets_dir->dir_path = ASSETS_FOLDER;
	assets_dir->dir_name = "Assets";
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

	ie_material = new ImporterMaterial;
	ie_scene = new ImporterScene;
	return false;
}

update_status ModuleResources::PreUpdate(float dt)
{
	if (App->scene_intro->playing == false)
	{
		float time_passed = timer.ReadSec(); //Time passed after update of data

		if (time_passed >= 1.0f)
		{
			CheckDirectoryUpdate(assets_dir);
			CheckFilesUpdate(assets_dir);
			timer.Start();
		}

		//Make sure anytime the selected dir is nullptr we set it to the assets (root) directory
		if (selected_dir == nullptr && assets_dir != nullptr)
			selected_dir = assets_dir;
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
	
	bool has_meta = false;
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

uint32 ModuleResources::ImportInternalFile(const char * new_file_in_assets)
{
	uint32 uid = 0; bool import_ok = false; std::string written_file;

	std::string full_path = App->file_system->GetPathToGameFolder(true) + new_file_in_assets;
	App->file_system->NormalizePath(full_path);

	uid = ImportFile(full_path.data());
	return uid;
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
		case Resource::MESH:       ret = (Resource*) new ResourceMesh(uid);      break; 
		case Resource::TEXTURE: ret = (Resource*) new ResourceTexture(uid);  break;
		case Resource::SCRIPT:    ret = (Resource*) new ResourceScript(uid);     break;
	}
	
	if (ret != nullptr) 
		resources[uid] = ret;
return ret;
}

int64 ModuleResources::GetTimestampFromMeta(const char * path, bool game_path)
{
	int64 ret = 0;

	std::string final_path = path;
	std::string base_path = App->file_system->GetBasePath();

	if (game_path)
	{
		base_path = App->SubtractString(base_path, "\\", true, true, false);
		base_path = App->SubtractString(base_path, "\\", true, true, true);
		base_path += "Game";
		App->file_system->NormalizePath(base_path);
		final_path = base_path.data() + final_path;
	}

	json loaded_file = App->jLoad.Load(final_path.data()); //Load the .meta as a json file 
	
	ret = loaded_file["Mod_Time"];
	
	return ret;
}

void ModuleResources::GetAllFilesWithExtension(std::string & extension, std::vector<std::string>& files, AbstractDir* directory)
{

	for (int i = 0; i < directory->files.size(); ++i)
	{
		std::string file_extension;
		App->file_system->SplitFilePath(directory->files[i].data(),nullptr,nullptr,&file_extension);

		if (!file_extension.compare(extension))
		{
			files.push_back(directory->dir_path + directory->files[i]);
		}
	}

	//Recursivity
	if (directory->sub_dirs.size() > 0)
	{
		for (int i = 0; i < directory->sub_dirs.size(); ++i)
		{
			GetAllFilesWithExtension(extension, files, directory->sub_dirs[i]);
		}
	}
}

void ModuleResources::ManageModifiedFile(std::string extension)
{
	std::string lua = "lua";
	if (!extension.compare(lua))
	{
		App->scripting->DoHotReloading();
	}
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

void ModuleResources::InitPopulateAssetsDir(AbstractDir* abs_dir)
{
	LOG("Loading Assets folder hierarchy.");
	std::vector<std::string> discovered_files, discovered_dirs;

	App->file_system->DiscoverFiles(abs_dir->dir_path.data(), discovered_files, discovered_dirs);

	for (int i = 0; i < discovered_files.size(); ++i)
	{

		//Chek if the file is a .meta, since we don't want to include them in the files displayed in the engine + we will update meta info with name + .meta
		std::string extension;
		App->file_system->SplitFilePath(discovered_files[i].data(),nullptr,nullptr,&extension);
		if ( !extension.compare(meta))
		{
			continue;
		}

		//Add the file to our in-code-custom-directory
		abs_dir->files.push_back(discovered_files[i]);

		//See if it has a meta
		std::string meta_path = abs_dir->dir_path + discovered_files[i] + ".meta";
		//meta_path = App->file_system->GetPathToGameFolder(true) + "/" + meta_path;
		if (App->file_system->Exists(meta_path.data()))
		{
			LOG("%s exists", meta_path.data());
			int64 ret = GetTimestampFromMeta(meta_path.data(), true);
			abs_dir->file_modtimes.push_back(ret);
		}
		else
		{
			std::string extension;
			App->file_system->SplitFilePath(discovered_files[i].data(),nullptr,nullptr,&extension);
			std::string lua = "lua";
			if (!extension.compare(lua))
			{
				App->scripting->ManageOrphanScript(abs_dir->dir_path + discovered_files[i]);
			}
		}
	}

	for (int i = 0; i < discovered_dirs.size(); ++i)
	{
		AbstractDir* new_dir = new AbstractDir;
		new_dir->parent_dir = abs_dir;
		new_dir->dir_name = discovered_dirs[i];
		new_dir->dir_path = abs_dir->dir_path + discovered_dirs[i] + "/";
		abs_dir->sub_dirs.push_back(new_dir);
	}

	for (int i = 0; i < abs_dir->sub_dirs.size(); ++i)
		InitPopulateAssetsDir(abs_dir->sub_dirs[i]);
}

void ModuleResources::CheckDirectoryUpdate(AbstractDir* abs_dir)
{
	std::vector<std::string> discovered_files, discovered_dirs;

	App->file_system->DiscoverFiles(abs_dir->dir_path.data(), discovered_files, discovered_dirs);

	int size_diff = abs_dir->sub_dirs.size() - discovered_dirs.size();
	if (size_diff == 0)	//If no subdirectory was added or removed
	{
		//Check if the name changed
	}
	else if (size_diff != 0)	// If sub directory added or removed
	{
		if (size_diff > 0)	// A directory was removed
		{
			//Check on the known directories which ones were deleted
			for (int i = 0; i < abs_dir->sub_dirs.size(); ++i)
			{
				if (!App->file_system->Exists(abs_dir->sub_dirs[i]->dir_path.data()))
				{
					LOG("[Info]: Directory %s and all its dependencies were removed", abs_dir->sub_dirs[i]->dir_path.data());
					RELEASE(abs_dir->sub_dirs[i]); // Delete the sub directory in memory (and all sub directories of the directory in "recursive" manner)
					abs_dir->sub_dirs.erase(abs_dir->sub_dirs.begin() + i);
				}
			}
		}
		else //A directory was added
		{
			// We will remove from the discovered vector all elements that are already known, it's not the most optimal way to do this, but at least we are doing some optimization
			// instead of just comparing all elements of a against elements of b each iteration
			for (int i = 0; i < abs_dir->sub_dirs.size(); ++i)
			{
				for (int j = 0; j < discovered_dirs.size(); ++j)
				{
					if (abs_dir->sub_dirs[i]->dir_name == discovered_dirs[j])
					{
						discovered_dirs.erase(discovered_dirs.begin() + j);
						break;
					}
				}
			}

			//We have discarded all common elements in discovered directories, so the array contains only the different dirs
			for (int i = 0; i < discovered_dirs.size(); ++i)
			{
				AbstractDir* new_dir = new AbstractDir;
				new_dir->parent_dir = abs_dir;
				new_dir->dir_name = discovered_dirs[i];
				new_dir->dir_path = abs_dir->dir_path + discovered_dirs[i] + "/";
				abs_dir->sub_dirs.push_back(new_dir);
				LOG("[Info]: Directory %s was created", new_dir->dir_path.data());
			}
		}
	}
	// Call Recursive
	for (int i = 0; i < abs_dir->sub_dirs.size(); ++i)
		CheckDirectoryUpdate(abs_dir->sub_dirs[i]);
}

void ModuleResources::CheckFilesUpdate(AbstractDir* abs_dir)
{
	std::vector<std::string> discovered_files, discovered_dirs;

	App->file_system->DiscoverFiles(abs_dir->dir_path.data(), discovered_files, discovered_dirs);

	int size_diff = abs_dir->files.size() - discovered_files.size();
	if (size_diff == 0)	//If no file was added or removed on the directory
	{
		//Check if the name changed or if the file changed
		if (abs_dir->file_modtimes.size() > 0)
		{
			for (int i = 0; i < abs_dir->files.size(); ++i)
			{
				if (i < abs_dir->file_modtimes.size())
				{
					std::string path = abs_dir->dir_path + abs_dir->files[i];
					int64 mod_time = App->file_system->GetFileModDate(path.data());
					if (abs_dir->file_modtimes[i] != mod_time)
					{
						LOG("%s was modified!", path.data());

						std::string extension;
						App->file_system->SplitFilePath(abs_dir->files[i].data(),nullptr,nullptr,&extension);
						
						for (int j = 0; j < extension_3D_file.size(); ++j)
						{
							if (!extension_3D_file[j].compare(extension))
							{
								std::string file_path = abs_dir->dir_path + abs_dir->files[i];
								ie_scene->EditMeta(file_path.data(), true);
							}
						}

						for (int j = 0; j < extension_texture.size(); ++j)
						{
							if (!extension_texture[j].compare(extension))
							{
								std::string file_path = abs_dir->dir_path + abs_dir->files[i];
								ie_material->EditMeta(file_path.data(),true);
							}
						}
						abs_dir->file_modtimes[i] = mod_time;
						ManageModifiedFile(extension);
					}
				}
			}
		}
	}
	else if (size_diff != 0)	// If file added or removed on the directory
	{
		if (size_diff > 0)	// A file was removed
		{
			std::string file_path;
			//Check on the known files which ones were deleted
			for (int i = 0; i < abs_dir->files.size(); ++i)
			{
				file_path = abs_dir->dir_path + abs_dir->files[i];
				if (!App->file_system->Exists(file_path.data()))
				{
					LOG("[Info]: File %s was removed from %s", abs_dir->files[i].data(), abs_dir->dir_path.data());
					abs_dir->files.erase(abs_dir->files.begin() + i);
				}
			}
		}
		else //A File was added
		{
			// We will remove from the discovered vector all elements that are already known, it's not the most optimal way to do this, but at least we are doing some optimization
			// instead of just comparing all elements of a against elements of b each iteration
			for (int i = 0; i < abs_dir->files.size(); ++i)
			{
				for (int j = 0; j < discovered_files.size(); ++j)
				{
					if (abs_dir->files[i] == discovered_files[j])
					{
						discovered_files.erase(discovered_files.begin() + j);
						break;
					}
				}
			}

			//We have discarded all common elements in discovered files, so the array contains only the different files
			for (int i = 0; i < discovered_files.size(); ++i)
			{
				abs_dir->files.push_back(discovered_files[i]);
				std::string meta_path = abs_dir->dir_path + discovered_files[i] + ".meta";
				App->file_system->GetPathToGameFolder(&meta_path);
				if (App->file_system->Exists(meta_path.data()))
				{
					LOG("%s exists", meta_path.data());
				}
				LOG("[Info]: File %s was added at %s", discovered_files[i].data(), abs_dir->dir_path.data());
			}
		}
	}

	// Call Recursive
	for (int i = 0; i < abs_dir->sub_dirs.size(); ++i)
		CheckFilesUpdate(abs_dir->sub_dirs[i]);
}
