#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <string>
#include "libs/imgui/imgui.h"

class EditorWindow
{
public:
	EditorWindow(const char* name, bool startEnabled, ImGuiWindowFlags flags = ImGuiWindowFlags_None) : name(name), show_window(startEnabled), flags(flags) {};
	virtual ~EditorWindow() {};

public:
	void RequestUpdate()	// Called from the manager for all windows
	{
		if (show_window) {
			PreUpdate();
			ImGui::Begin(name.c_str(), &show_window, flags);
			Update();
			ImGui::End();
		}
	}
	virtual void PreUpdate() {};	// Any preparations before starting the window
	virtual void Update() = 0;		// Whatever the window does must be defined in the child class

public:
	std::string name;
	bool show_window = false;
	ImGuiWindowFlags flags;

};

#endif // !EDITORWINDOW_H