#ifndef EDITORHIERARCHY_H
#define EDITORHIERARCHY_H

#include "EditorWindow.h"

class GameObject;

class EditorHierarchy : public EditorWindow
{
public:
	EditorHierarchy(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
	void DrawPopUpWindow();

	//Manage the draw and selction of Game Objects in the hierarchy
	void ManageGameObject(GameObject* go, int& treenode_id);

public:
	int focus_node = -1;
	bool open_pop_up = false;
	bool pop_up_hovered = false;

private:
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool selected_go_already = false;
	GameObject* dragged_go = nullptr;

};

#endif // !EDITORHIERARCHY_H