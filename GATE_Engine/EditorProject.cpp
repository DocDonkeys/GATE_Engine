#include "EditorProject.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleResources.h"

// Memory Leak Detection
#include "MemLeaks.h"

EditorProject::EditorProject(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorProject::Update()
{
	selected_dir_already = false;
	int id = 0;

	ImGui::BeginChild("Project", ImVec2(300, 0), true);
	PrintAssetsHierarchy(App->resources->assets_dir, id);
	ImGui::EndChild();

	
	ImGui::SameLine();

	ImGui::BeginChild("Assets", ImVec2(0, 0), true);
		for (int i = 0; i < 20; ++i)
			ImGui::Text("Test Text, let's figure this out ");

	ImGui::EndChild();
}

void EditorProject::DrawAssetsLayout()
{
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