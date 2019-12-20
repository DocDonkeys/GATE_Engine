#include "ImporterMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "TextureLoader.h"
#include "ResourceTexture.h"
#include "ModuleResources.h"

ImporterMaterial::ImporterMaterial()
{
}

ImporterMaterial::~ImporterMaterial()
{
}

bool ImporterMaterial::Export(const char * path, std::string & output_file, const ImportExportData * ie_data, const char * filename)
{
	return false;
}

bool ImporterMaterial::Load(const char * full_path, ResourceTexture* tex)
{
	bool ret = false;
	std::string path, uid_string;
	App->file_system->SplitFilePath(full_path,&path, &uid_string);
	
	//Check if texture is already on the map
	//if (tex->GetUID() != std::stoul(uid_string))
	//{
		tex = App->texture_loader->LoadTextureFile(full_path);
		if (tex != nullptr)
			ret = true;
	//}
	//else
	//{
	//	tex = (ResourceTexture*)App->resources->Get(std::stoul(uid_string));
	//	if(tex!=nullptr)
	//	ret = true;
	//}

	return false;
}

ResourceTexture * ImporterMaterial::LoadTexture(const char * path, bool duplicate,uint32 force_uid)
{
	ResourceTexture* ret = nullptr;

	//Check if the texture is already on the resources map
	ret = (ResourceTexture*)App->resources->Get(force_uid);
	if (force_uid != 0 && ret != nullptr)
	{
		ret->AddReference();
		return ret;
	}
	else //Load texture
	{
		ret = App->texture_loader->LoadTextureFile(path,duplicate,force_uid);
		ret->AddReference();
	}

	return ret;
}

bool ImporterMaterial::CreateMeta(const char * original_file_full_path, ImportExportData * ie_data)
{
	json file; //File to save
	std::string path, filename, extension;
	App->file_system->SplitFilePath(original_file_full_path, &path, &filename, &extension);
	int64 mod_time = App->file_system->GetFileModDate(original_file_full_path);

	//Data saving
	file["Path"] = ie_data->meta_path.data();

	//Store the UID (we can extract from meta_path)
	std::string file_UID;
	App->file_system->SplitFilePath(ie_data->meta_path.data(),nullptr,&file_UID,nullptr);
	file_UID = App->SubtractString(file_UID, ".", true, true);
	file_UID = App->SubtractString(file_UID,"t",false,false);

	file["UID"] = file_UID.data();
	file["Mod Time"] = mod_time;



	//Convert to buffer
	std::string data = App->jLoad.JsonToString(file);
	char* buffer = (char*)data.data();
	std::string output;

	//Save File
	App->file_system->SaveUnique(output, buffer, data.length(), path.data(), filename.data(), "meta");

	return false;
}

ResourceTexture* ImporterMaterial::LoadMeta(const char * full_path, bool game_path)
{
	ResourceTexture* ret = nullptr;
	std::string path = full_path;
	std::string base_path = App->file_system->GetBasePath();

	if (game_path)
	{
		base_path = App->SubtractString(base_path, "\\", true, true, false);
		base_path = App->SubtractString(base_path, "\\", true, true, true);
		base_path += "Game";
		App->file_system->NormalizePath(base_path);
		path = base_path + path;
	}

	json loaded_file = App->jLoad.Load(path.data()); //Load the .meta as a json file 

	std::string texture_path = loaded_file["Path"];
	texture_path = base_path + texture_path;
	//Further importing options could go here
	std::string UID = loaded_file["UID"];
	uint32 uid = std::stoul(UID);

	ret = App->texture_loader->importer.LoadTexture(texture_path.data(), false,uid);
	//ret = App->texture_loader->LoadTextureFile(texture_path.data(), false);
	return ret;
}
