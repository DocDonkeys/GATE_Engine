#ifndef EDITORINSPECTOR_H
#define EDITORINSPECTOR_H

#include "EditorWindow.h"

class EditorInspector : public EditorWindow
{
public:
	EditorInspector(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
};

#endif // !EditorInspector