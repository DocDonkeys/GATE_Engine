#include "ImporterScene.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
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

bool ImporterScene::Load(const char * full_path)
{
	bool ret = false;

	std::vector<GameObject*> gos;
	json loaded_file = App->jLoad.Load(full_path); //Load the json file 

	//Load the .scene into a buffer
	for (json::iterator it = loaded_file.begin(); it != loaded_file.end(); ++it)
	{
		GameObject* go = App->scene_intro->CreateEmptyGameObject();

		const char* go_name = it.key().data();

		go->name = go_name;
		
		std::string UID = loaded_file[go_name]["UID"];
		go->UID = std::stoi(UID);
		std::string parent_UID = loaded_file[go_name]["Parent UID"];
		go->parent_UID = std::stoi(parent_UID);

		std::string active = loaded_file[go_name]["Active"];
		go->active = (bool)std::stoi(active);
		std::string static_obj = loaded_file[go_name]["Static"];
		go->staticObj = (bool)std::stoi(static_obj);

		gos.push_back(go);
	}

	//Load parenting
	for (int i = 0; i < gos.size(); ++i)
	{
		if(gos[i]->parent_UID == 0)
		GOFunctions::ReParentGameObject(gos[i], App->scene_intro->root);
		else
		{
				for (int j = 0; j < gos.size(); ++j)
				{
					if (gos[j]->UID == gos[i]->parent_UID)
					{
						GOFunctions::ReParentGameObject(gos[i], gos[j]);
						break;
					}
				}
		}
	}


	return ret;
}

std::string ImporterScene::SaveScene(const GameObject * root_go, std::string & scene_name, FileType file_type)
{
	json file; //File to save

	std::vector<const GameObject*> gos;
	GOFunctions::FillArrayWithChildren(gos, root_go, true);

	//Iterate all game objects
	for (int i = 0; i < gos.size(); ++i)
	{
		const char* go_name = gos[i]->name.data();
		file[go_name];
		file[go_name]["UID"] = std::to_string(gos[i]->UID);
		file[go_name]["Parent UID"] = std::to_string(gos[i]->parent_UID);

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

	std::string data = App->jLoad.JsonToString(file);
	char* buffer = (char*)data.data();
	std::string output;
	App->file_system->SaveUnique(output,buffer,data.length(),LIBRARY_FOLDER,scene_name.data(),"scene");
	App->jLoad.Save(file, "settings/scene_test.json");

	return std::string();
}
