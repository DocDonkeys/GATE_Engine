#ifndef EDITORGAME_H
#define EDITORGAME_H

#include "EditorWindow.h"

class EditorGame : public EditorWindow
{
public:
	EditorGame(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
};

#endif // !EDITORGAME_H