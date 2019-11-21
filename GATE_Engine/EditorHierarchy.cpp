#include "EditorHierarchy.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

// Memory Leak Detection
#include "MemLeaks.h"

EditorHierarchy::EditorHierarchy(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorHierarchy::Update()
{
	selected_go_already = false;
	if (open_pop_up)
		DrawPopUpWindow();

	//We manage the drawing and editing of the hierarchy
	int treenode_id = 0;
	for (int i = 0; i < App->scene_intro->root->children.size(); ++i)
		ManageGameObject(App->scene_intro->root->children[i], treenode_id);
}

//Draw the Pop up window for stuff like deleting game objects, duplicating them etc.
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
	//Prepare flags for the treenode, depending on if it's selected and/or has children
	ImGuiTreeNodeFlags tmp_flags = base_flags;
	if (go == App->scene_intro->selected_go)
		tmp_flags = base_flags | ImGuiTreeNodeFlags_Selected;

	if (go->children.size() == 0)
		tmp_flags = tmp_flags | ImGuiTreeNodeFlags_Leaf;

	//Increment the ID
	treenode_id++;

	//Print GameObjects Hierarchy
	if (ImGui::TreeNodeEx((void*)(intptr_t)treenode_id, tmp_flags, go->name.data()))
	{
		//Determine mouse interaction outcomes
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

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("DUMMY_CELL", go,sizeof(GameObject));
			ImGui::Text(" %s ", go->name.data());
			dragged_go = go; // We store the gameobject that should be carried by the payload
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DUMMY_CELL"))
			{
				GOFunctions::ReParentGameObject(dragged_go, go); //We assign the gameobject that should have been carried by the payload
				dragged_go = nullptr;
			}
			ImGui::EndDragDropTarget();
		}
		
		//Keep calling the recursive method with children if any
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

