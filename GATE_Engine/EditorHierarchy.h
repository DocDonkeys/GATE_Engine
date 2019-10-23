#ifndef EDITORHIERARCHY_H
#define EDITORHIERARCHY_H

#include "EditorWindow.h"

class EditorHierarchy : public EditorWindow
{
public:
	EditorHierarchy(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
};

#endif // !EDITORHIERARCHY_H