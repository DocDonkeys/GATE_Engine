#ifndef EDITORSCENE_H
#define EDITORSCENE_H

#include "EditorWindow.h"

class EditorScene : public EditorWindow
{
public:
	EditorScene(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
};

#endif // !EDITORSCENE_H