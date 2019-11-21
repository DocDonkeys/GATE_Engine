#include "ImporterMaterial.h"
#include "ResourceTexture.h"
#include "TextureLoader.h"
#include "Application.h"

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
