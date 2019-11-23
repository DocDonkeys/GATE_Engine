#include "ComponentMaterial.h"
#include "Application.h"
#include "TextureLoader.h"
#include "ModuleFileSystem.h"

ComponentMaterial::ComponentMaterial() : Component()
{
	type = COMPONENT_TYPE::MATERIAL;
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Enable()
{
	active = true;

	if (checkers_texture_id == 0)
		checkers_texture_id = App->texture_loader->GetDefaultId();
}

void ComponentMaterial::Disable()
{
	active = false;
}

void ComponentMaterial::Save(json & file)
{
	file["UID"] = this->UID;
	file["Active"] = this->active;
	file["Checkers active"] = this->use_default_texture;

	//Construct string of path to file
	//Textures are saved into library/textures whenever they are loaded for the 1st time, using _t + its UID as filename
	std::string full_path = "Game";
	full_path += LIBRARY_TEXTURES_FOLDER;
	full_path += "_t";
	bool has_texture = (loaded_texture != nullptr);
	file["Loaded Texture"] = std::to_string(has_texture);
	if (loaded_texture != nullptr)
	{
		full_path += std::to_string(this->loaded_texture->GetUID());
		full_path += ".dds";

		file["Path"] = full_path.data();
	}
}

void ComponentMaterial::Load(json & file)
{
	std::string absolute = App->file_system->GetBasePath();
	absolute = App->SubtractString(absolute,"\\",true,true,false);
	absolute = App->SubtractString(absolute, "\\", true, true, true);
	std::string bool_has_tex = file["Loaded Texture"];
	bool has_texture = std::stoi(bool_has_tex);

	if (has_texture)
	{
		std::string relative = file["Path"];
		std::string full_path = absolute + relative;
		App->file_system->NormalizePath(full_path);

		this->loaded_texture = App->texture_loader->LoadTextureFile(full_path.data());
		active_texture = loaded_texture;
	}
}

void ComponentMaterial::AssignTexture(ResourceTexture* texture)
{
	loaded_texture = texture;
	active_texture = loaded_texture;
}
