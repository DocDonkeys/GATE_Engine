#include "EditorHierarchy.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

EditorHierarchy::EditorHierarchy(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorHierarchy::Update()
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth; //| ImGuiTreeNodeFlags_Selected;
	for (int i = 0; i < App->scene_intro->game_objects.size(); ++i)
	{
		bool node_open = false;
		ImGuiTreeNodeFlags tmp_flags = base_flags;
		if (focus_node == i)
		{
			tmp_flags = base_flags | ImGuiTreeNodeFlags_Selected;
		}

		bool is_open = ImGui::TreeNodeEx((void*)(intptr_t)i, tmp_flags, App->scene_intro->game_objects[i]->name.data());

		if (is_open)
		{
			node_open = true;
			ImGui::TreePop();
		} 
		
		if (ImGui::IsItemClicked())
			focus_node = i;
	}
	
}