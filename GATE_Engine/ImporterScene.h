#ifndef __IMPORTER_SCENE_H__
#define __IMPORTER_SCENE_H__

#include "Importer.h"
#include <string>

class GameObject;

enum class FileType
{
	MODEL = 0,
	SCENE
	//If we were to do prefabs, it would go here
};

class ImporterScene : public Importer
{
public:
	ImporterScene();
	~ImporterScene();

public:
	bool Export(const char* path, std::string& output_file, const ImportExportData* ie_data, const char* filename = "");
	bool LoadScene(const char* full_path, FileType file_type);
	bool CreateMeta(const char* original_file_full_path, ImportExportData* ie_data);
	bool LoadMeta(const char* full_path, bool game_path = false);
	//Since .model will be same as a scene but uneditable, we can use the same function but determining in which format we want to save
	std::string SaveScene(const GameObject* root_go, std::string& scene_name, FileType file_type);
	bool EditMeta(const char* full_path, bool game_path = false);
};


#endif // !__IMPORTER_SCENE_H__

