#include "ImporterScene.h"
#include "Application.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include <vector>

ImporterScene::ImporterScene()
{
}

ImporterScene::~ImporterScene()
{
}

bool ImporterScene::Export(const char * path, std::string & output_file, const ImportExportData * ie_data, const char * filename)
{
	return false;
}

std::string ImporterScene::SaveScene(const GameObject * root_go, std::string & scene_name, FileType file_type)
{
	json file; //File to save

	std::vector<const GameObject*> gos;
	GOFunctions::FillArrayWithChildren(gos, root_go, true);

	//TEST: create a file with basic contents
	//Iterate all game objects
	for (int i = 0; i < gos.size(); ++i)
	{
		const char* go_name = gos[i]->name.data();
		file[go_name];
		file[go_name]["UID"] = std::to_string(gos[i]->UID);
		file[go_name]["Active"] = std::to_string(gos[i]->active);
		file[go_name]["Static"] = std::to_string(gos[i]->staticObj);

		if(gos[i]->parent != nullptr) // Should only happen for the rootnode of the scene
		file[go_name]["Parent"] = std::to_string(gos[i]->parent->UID);

		//This pointers will be used to store components, can't remember of a cleaner way to do this right now :(
		ComponentTransform* trans_comp = nullptr;
		ComponentMesh* mesh_comp = nullptr;
		ComponentMaterial* mat_comp = nullptr;
		ComponentCamera*cam_comp = nullptr;

		//Save Components. Although for now we only have up to 4 components, we need to prepare it for an unlimited number of components and types
		for (int j = 0; j < gos[i]->components.size(); ++j)
		{
			Component* component = gos[i]->components[j];

			switch (component->type)
			{
			case COMPONENT_TYPE::TRANSFORM:
				trans_comp = (ComponentTransform*)component;

				file[go_name]["Components"]["Transform"] = "no_path_just_test";
				break;

			case COMPONENT_TYPE::MESH:
				
				mesh_comp = (ComponentMesh*)component;

				file[go_name]["Components"]["Mesh"] = "no_path_just_test";
				break;

			case COMPONENT_TYPE::MATERIAL:
				mat_comp = (ComponentMaterial*)component;

				file[go_name]["Components"]["Material"] = "no_path_just_test";
				break;

			case COMPONENT_TYPE::CAMERA:
				cam_comp = (ComponentCamera*)component;

				file[go_name]["Components"]["Camera"] = "no_path_just_test";
				break;

				//Future components like scripts and audio should go here
			default:
				break;
			}
		}
	}
	std::string path = LIBRARY_FOLDER;
	path += scene_name;
	App->jLoad.Save(file, "settings/scene_test.json");

	return std::string();
}
