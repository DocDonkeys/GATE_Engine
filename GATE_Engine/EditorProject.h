#ifndef EDITORPROJECT_H
#define EDITORPROJECT_H

#include "EditorWindow.h"

class EditorProject : public EditorWindow
{
public:
	EditorProject(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
	void DrawAssetsLayout();
};

#endif // !EDITORPROJECT_H