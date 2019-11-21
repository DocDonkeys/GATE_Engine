#include "EditorToolbar.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

// Memory Leak Detection
#include "MemLeaks.h"

EditorToolbar::EditorToolbar(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorToolbar::Update()
{
	if (ImGui::RadioButton("Drag", &App->scene_intro->toolMode, (int)tool_mode::DRAG)) {	//CHANGE/FIX: Do what's needed for tool change
	
	}; ImGui::SameLine();
	if (ImGui::RadioButton("Move", &App->scene_intro->toolMode, (int)tool_mode::MOVE)) {
	
	}; ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", &App->scene_intro->toolMode, (int)tool_mode::ROTATE)) {
	
	}; ImGui::SameLine();
	if (ImGui::RadioButton("Scale", &App->scene_intro->toolMode, (int)tool_mode::SCALE)) {
	
	}; ImGui::SameLine();
	if (ImGui::RadioButton("Rect", &App->scene_intro->toolMode, (int)tool_mode::RECT)) {
	
	}; ImGui::SameLine();
	if (ImGui::RadioButton("Multi", &App->scene_intro->toolMode, (int)tool_mode::MULTI)) {
	
	}; ImGui::SameLine(App->window->window_width / 2 - 100);

	ImGui::Button("Play"); ImGui::SameLine();	//IMPROVE: ImageButtons
	ImGui::Button("Pause"); ImGui::SameLine();
	ImGui::Button("Frame"); ImGui::SameLine();
}