#include "ImporterMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "TextureLoader.h"
#include "ResourceTexture.h"

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

bool ImporterMaterial::CreateMeta(const char * original_file_full_path, ImportExportData * ie_data)
{
	json file; //File to save
	std::string path, filename, extension;
	App->file_system->SplitFilePath(original_file_full_path, &path, &filename, &extension);

	//Data saving
	file["Path"] = original_file_full_path;

	//Convert to buffer
	std::string data = App->jLoad.JsonToString(file);
	char* buffer = (char*)data.data();
	std::string output;

	//Save File
	App->file_system->SaveUnique(output, buffer, data.length(), path.data(), filename.data(), "meta");

	return false;
}
