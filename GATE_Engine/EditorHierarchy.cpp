#include "EditorHierarchy.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

EditorHierarchy::EditorHierarchy(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorHierarchy::Update()
{
	selected_go_already = false;
	if (open_pop_up)
		DrawPopUpWindow();

	int treenode_id = 0;
	
	for (int i = 0; i < App->scene_intro->root->children.size(); ++i)
	{
		ManageGameObject(App->scene_intro->root->children[i], treenode_id);
	}
}

void EditorHierarchy::DrawPopUpWindow()
{
	if (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(2))
		open_pop_up = false;
	else if (ImGui::IsMouseClicked(1))
		open_pop_up = false;

	ImGui::OpenPopup("Hierarchy Tools");
		if (ImGui::BeginPopup("Hierarchy Tools"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				if (App->scene_intro->selected_go != nullptr)
				App->scene_intro->DestroyGameObject(App->scene_intro->selected_go);
				open_pop_up = false;
			}

			ImGui::EndPopup();
		}
}

void EditorHierarchy::ManageGameObject(GameObject* go, int& treenode_id)
{
	ImGuiTreeNodeFlags tmp_flags = base_flags;
	if (go == App->scene_intro->selected_go)
		tmp_flags = base_flags | ImGuiTreeNodeFlags_Selected;

	if (go->children.size() == 0)
		tmp_flags = tmp_flags | ImGuiTreeNodeFlags_Leaf;


	treenode_id++;

	//Print GameObjects Hierarchy
	if (ImGui::TreeNodeEx((void*)(intptr_t)treenode_id, tmp_flags, go->name.data()))
	{
		if (ImGui::IsItemClicked(0) && selected_go_already == false)
		{
				App->scene_intro->selected_go = go;
				selected_go_already = true;
		}
		else if (ImGui::IsItemClicked(1) && ImGui::IsWindowHovered() && selected_go_already == false) 
		{
			open_pop_up = true;
			App->scene_intro->selected_go = go;
		}

		if (go->children.size() > 0)
		{
			for (int i = 0; i < go->children.size(); ++i)
			{
				ManageGameObject(go->children[i], treenode_id);
			}
		}

		ImGui::TreePop();
	}
}
