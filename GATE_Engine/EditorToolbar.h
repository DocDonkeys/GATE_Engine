#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include "EditorWindow.h"

class EditorToolbar : public EditorWindow
{
public:
	EditorToolbar(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
};

#endif // !EDITORTOOLBAR_H