#include "ImporterScene.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleFileSystem.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

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

bool ImporterScene::LoadScene(const char * full_path, FileType file_type)
{
	bool ret = false;

	std::vector<GameObject*> gos;
	json loaded_file = App->jLoad.Load(full_path); //Load the .scene as a json file 

	//Iterate the file and assign data
	for (json::iterator it = loaded_file.begin(); it != loaded_file.end(); ++it)
	{
		GameObject* go = App->scene_intro->CreateEmptyGameObject();

		const char* go_name = it.key().data();
		go->name = go_name;
		
		std::string UID = loaded_file[go_name]["UID"];
		go->UID = std::stoul(UID);
		std::string parent_UID = loaded_file[go_name]["Parent UID"];
		go->parent_UID = std::stoul(parent_UID);

		std::string active = loaded_file[go_name]["Active"];
		go->active = (bool)std::stoi(active);
		std::string static_obj = loaded_file[go_name]["Static"];
		go->staticObj = (bool)std::stoi(static_obj);

		//Vars we can't declare inside a switch but needed for data loading
		ComponentTransform* trans = nullptr;
		ComponentMesh* mesh = nullptr;
		ComponentMaterial* mat = nullptr;

		float local_mat[16] = {0};
		float global_mat[16] = {0};

		std::string mat_type;
		std::string mat_num;
		std::string mat_val;

		//Check for components 
		json loaded_components = loaded_file[go_name]["Components"];
		for (json::iterator it_comp = loaded_components.begin(); it_comp != loaded_components.end(); ++it_comp)
		{
			std::string comp = it_comp.key().data();
			COMPONENT_TYPE comp_type = (COMPONENT_TYPE)std::stoi(comp);

			//Switch for each case
			switch (comp_type)
			{
			case COMPONENT_TYPE::TRANSFORM:
				//All GameObjects will ALWAYS have 1 transform component (just 1)
				trans = (ComponentTransform*)go->GetComponent(COMPONENT_TYPE::TRANSFORM);

				mat_type = "Gnum"; //Load Global
				for (int k = 0; k < 16; ++k)
				{
					mat_num = std::to_string(k);
					mat_val = mat_type + mat_num;
					global_mat[k] = loaded_components[comp][mat_val.data()];
				}

				mat_type = "Lnum"; //Load Local
				for (int k = 0; k < 16; ++k)
				{
					mat_num = std::to_string(k);
					mat_val = mat_type + mat_num;
					local_mat[k] = loaded_components[comp][mat_val.data()];
				}

				trans->Import(local_mat,global_mat); //Manage new local and global in component
				break;

			case COMPONENT_TYPE::MESH:
				mesh = (ComponentMesh*)go->CreateComponent(COMPONENT_TYPE::MESH);
				if (mesh != nullptr)
				mesh->Load(loaded_file[go_name]["Components"][std::to_string((int)comp_type)]);
				break;

			case COMPONENT_TYPE::MATERIAL:
				mat = (ComponentMaterial*)go->CreateComponent(COMPONENT_TYPE::MATERIAL);
				if (mat != nullptr)
					mat->Load(loaded_file[go_name]["Components"][std::to_string((int)comp_type)]);
				break;
			case COMPONENT_TYPE::CAMERA:
				break;
			default:
				break;
			}
		}
		gos.push_back(go);
	}

	//Load parenting
	GameObject* root = nullptr;
	for (int i = 0; i < gos.size(); ++i)
	{
		if (gos[i]->parent_UID == 0)
		{
			if(file_type != FileType::SCENE)
			GOFunctions::ReParentGameObject(gos[i], App->scene_intro->root);

			root = gos[i];
		}
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

	if (file_type == FileType::SCENE)
	{
		App->scene_intro->ChangeScene(root);
	}

	return ret;
}

bool ImporterScene::CreateMeta(const char * original_file_full_path, ImportExportData * ie_data)
{
	json file; //File to save
	std::string path, filename, extension;
	App->file_system->SplitFilePath(original_file_full_path,&path,&filename,&extension);

	//Data saving
	file["Path"] = ie_data->meta_path.data();

	//Convert to buffer
	std::string data = App->jLoad.JsonToString(file);
	char* buffer = (char*)data.data();
	std::string output;

	//Save File
	App->file_system->SaveUnique(output, buffer, data.length(), path.data(), filename.data(), "meta");
	return false;
}

bool ImporterScene::LoadMeta(const char * full_path, bool game_path)
{
	std::string path = full_path;
	std::string base_path = App->file_system->GetBasePath();

	if (game_path)
	{
		base_path = App->SubtractString(base_path, "\\", true, true, false);
		base_path = App->SubtractString(base_path, "\\", true, true, true);
		base_path += "Game";
		App->file_system->NormalizePath(base_path);
		path = base_path  + path;
	}

	json loaded_file = App->jLoad.Load(path.data()); //Load the .scene as a json file 

	std::string model_path = loaded_file["Path"];
	model_path = base_path + model_path;
	//Further importing options could go here

	//Load the .model
	App->scene_intro->scene_ie.LoadScene(model_path.data(),FileType::MODEL);

	return false;
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

		//if(gos[i]->parent == nullptr) // Should only happen for the rootnode of the scene
		//file[go_name]["Parent"] = std::to_string(gos[i]->parent->UID);

		//This pointers will be used to store components, can't remember of a cleaner way to do this right now :(
		ComponentTransform* trans_comp = nullptr;
		ComponentMesh* mesh_comp = nullptr;
		ComponentMaterial* mat_comp = nullptr;
		ComponentCamera*cam_comp = nullptr;

		float local_mat[16] = {0};
		float global_mat[16] = {0};

		std::string mat_type;
		std::string mat_num;
		std::string mat_val;
		json::iterator json_it;

		//Save Components. Although for now we only have up to 4 components, we need to prepare it for an unlimited number of components and types
		for (int j = 0; j < gos[i]->components.size(); ++j)
		{
			Component* component = gos[i]->components[j];

			switch (component->type)
			{
			case COMPONENT_TYPE::TRANSFORM:
				trans_comp = (ComponentTransform*)component;
				trans_comp->Export(local_mat, global_mat);

				mat_type = "Lnum";
				for (int k = 0; k < 16; ++k)
				{
					mat_num = std::to_string(k);
					mat_val = mat_type + mat_num;
					file[go_name]["Components"][std::to_string((int)component->type)][mat_val.data()] = local_mat[k];
				}

				mat_type = "Gnum";
				for (int k = 0; k < 16; ++k)
				{
					mat_num = std::to_string(k);
					mat_val = mat_type + mat_num;
					file[go_name]["Components"][std::to_string((int)component->type)][mat_val.data()] = global_mat[k];
				}
				break;

			case COMPONENT_TYPE::MESH:
				mesh_comp = (ComponentMesh*)component;
				mesh_comp->Save(file[go_name]["Components"][std::to_string((int)component->type)]); // Calling SAVE from Component Mesh 
				break;

			case COMPONENT_TYPE::MATERIAL:
				mat_comp = (ComponentMaterial*)component;
				mat_comp->Save(file[go_name]["Components"][std::to_string((int)component->type)]);
				break;

			case COMPONENT_TYPE::CAMERA:
				cam_comp = (ComponentCamera*)component;

				file[go_name]["Components"][std::to_string((int)component->type)] = "no_path_just_test";
				break;

				//Future components like scripts and audio should go here
			default:
				break;
			}
		}
	}

	std::string data = App->jLoad.JsonToString(file);
	char* buffer = (char*)data.data();
	std::string output;

	if (file_type == FileType::SCENE)
		App->file_system->SaveUnique(output,buffer,data.length(), ASSETS_FOLDER,scene_name.data(),"scene");
	else if (file_type == FileType::MODEL)
		App->file_system->SaveUnique(output, buffer, data.length(), LIBRARY_MODEL_FOLDER, scene_name.data(), "model");

	return output;
}
