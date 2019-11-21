#include "EditorProject.h"
#include "ModuleEditor.h"
#include "Application.h"

// Memory Leak Detection
#include "MemLeaks.h"

EditorProject::EditorProject(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorProject::Update()
{

}