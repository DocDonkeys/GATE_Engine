#include "EditorProject.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ResourceTexture.h"

// Memory Leak Detection
#include "MemLeaks.h"

EditorProject::EditorProject(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {}
;

bool EditorProject::Start()
{
	//Get the textures for the UI icons
	std::string path = ASSETS_FOLDER;
	path += "Engine_UI/folder.png";
	std::string model_path = ASSETS_FOLDER;
	model_path += "Engine_UI/model.png";

	folder_tex = (ResourceTexture*)App->resources->Get(App->resources->ImportInternalFile(path.data()));
	object_tex = (ResourceTexture*)App->resources->Get(App->resources->ImportInternalFile(model_path.data()));

	return true;
}

void EditorProject::Update()
{
	selected_dir_already = false;
	int id = 0;

	ImGui::Columns(2, "mixed");
	if (initial_width != 0.0f)
	{
		ImGui::SetColumnWidth(0, initial_width);
		initial_width = 0.0f;
	}

	ImGui::Text("Assets hierarchy");
	ImGui::Separator();
	PrintAssetsHierarchy(App->resources->assets_dir, id);
	
	ImGui::NextColumn();

	ImGui::Text("Assets");
	ImGui::Separator();

	DrawAssetsLayout(App->resources->selected_dir);
}

void EditorProject::DrawAssetsLayout(AbstractDir* selected_dir)
{
	for (int i = 0; i < selected_dir->sub_dirs.size(); ++i)
	{
		ImGui::BeginGroup();
		if(folder_tex != nullptr)
			ImGui::Image((GLuint*)folder_tex->id, ImVec2(60, 60));
		else
			ImGui::Image(0, ImVec2(60, 60));

		if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered() && !selected_dir->sub_dirs.empty())
		{
			App->resources->selected_dir = selected_dir->sub_dirs[i];
		}

		ImGui::TextWrapped(selected_dir->sub_dirs[i]->dir_name.data());

		
		ImGui::EndGroup();
		ImGui::SameLine();
	}

	for (int i = 0; i < selected_dir->files.size(); ++i)
	{
		ImGui::BeginGroup();
		if (folder_tex != nullptr)
			ImGui::Image((GLuint*)object_tex->id, ImVec2(60, 60));
		else
			ImGui::Image(0, ImVec2(60, 60));

		ImGui::TextWrapped(selected_dir->files[i].data());
		ImGui::EndGroup();
		ImGui::SameLine();
	}
}

void EditorProject::PrintAssetsHierarchy(AbstractDir* abs_dir, int& treenode_id)
{
	//Prepare flags for the treenode, depending on if it's selected and/or has sub directories
	ImGuiTreeNodeFlags tmp_flags = base_flags;
	if (abs_dir == App->resources->selected_dir)
		tmp_flags = base_flags | ImGuiTreeNodeFlags_Selected;

	if (abs_dir->sub_dirs.size() == 0)
		tmp_flags = tmp_flags | ImGuiTreeNodeFlags_Leaf;

	//Increment the ID
	treenode_id++;

	//Print GameObjects Hierarchy
	if (ImGui::TreeNodeEx((void*)(intptr_t)treenode_id, tmp_flags, abs_dir->dir_name.data()))
	{
		//Determine mouse interaction outcomes
		if (ImGui::IsItemClicked(0) && selected_dir_already == false)
		{
			App->resources->selected_dir = abs_dir;
			selected_dir_already = true;
		}

		//Keep calling the recursive method with children if any
		if (abs_dir->sub_dirs.size() > 0)
		{
			for (int i = 0; i < abs_dir->sub_dirs.size(); ++i)
			{
				PrintAssetsHierarchy(abs_dir->sub_dirs[i], treenode_id);
			}
		}

		ImGui::TreePop();
	}
}