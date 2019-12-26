#include "ImporterScript.h"
#include "Application.h"
#include "ModuleFileSystem.h"

ImporterScript::ImporterScript()
{
}

ImporterScript::~ImporterScript()
{
}

bool ImporterScript::CreateMeta(const char * original_file_full_path, ImportExportData * ie_data)
{
	json file; //File to save
	std::string path, filename, extension;
	App->file_system->SplitFilePath(original_file_full_path, &path, &filename, &extension);
	int64 mod_time = App->file_system->GetFileModDate(original_file_full_path);

	//Data saving
	file["Path"] = ie_data->meta_path.data();
	file["Mod_Time"] = mod_time;

	//Convert to buffer
	std::string data = App->jLoad.JsonToString(file);
	char* buffer = (char*)data.data();
	std::string output;

	//Save File
	App->file_system->SaveUnique(output, buffer, data.length(), path.data(), filename.data(), "meta");
	return true;
}

bool ImporterScript::LoadMeta(const char * full_path, bool game_path)
{
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

	json loaded_file = App->jLoad.Load(path.data()); //Load the .scene as a json file 

	//Here we woul put any loading capabilities, for now we will merely use the .meta to notice the engine if a script has been modified
	// Whenever we have time we could try to do the function dumping of LUA to store scripts as binary representations

	return true;
}

bool ImporterScript::ImportableResource(const char * full_path)
{
	return false;
}
