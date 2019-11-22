#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include "EditorWindow.h"

class EditorToolbar : public EditorWindow
{
public:
	EditorToolbar(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();

private:
	const char* handlePositionStr[2] = { "Center", "Pivot" };
	const char* handleRotationStr[2] = { "Local", "Global" };
};

#endif // !EDITORTOOLBAR_H