#include "EditorScene.h"
#include "ModuleEditor.h"
#include "Application.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif
EditorScene::EditorScene(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorScene::Update()
{

}