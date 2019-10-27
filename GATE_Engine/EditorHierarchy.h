#ifndef EDITORHIERARCHY_H
#define EDITORHIERARCHY_H

#include "EditorWindow.h"

class EditorHierarchy : public EditorWindow
{
public:
	EditorHierarchy(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
	void DrawPopUpWindow();

public:
	int focus_node = -1;
	bool open_pop_up = false;
	bool pop_up_hovered = false;
};

#endif // !EDITORHIERARCHY_H