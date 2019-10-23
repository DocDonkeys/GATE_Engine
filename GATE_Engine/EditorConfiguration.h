#ifndef EDITORCONFIGURATION_H
#define EDITORCONFIGURATION_H

#include "EditorWindow.h"

class EditorConfiguration : public EditorWindow
{
public:
	EditorConfiguration(const char* name = "null", bool startEnabled = false, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	void Update();
};

#endif // !EDITORCONFIGURATION_H