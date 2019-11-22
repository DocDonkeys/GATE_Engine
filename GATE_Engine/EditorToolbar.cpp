#include "EditorToolbar.h"
#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleWindow.h"

// Memory Leak Detection
#include "MemLeaks.h"

EditorToolbar::EditorToolbar(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorToolbar::Update()
{
	float windowCenter = ImGui::GetWindowWidth() / 2;

	// Tools
	if (ImGui::RadioButton("Drag", &App->scene_intro->toolMode, (int)tool_mode::DRAG)) {	//CHANGE/FIX: Do what's needed for tool change
	
	} ImGui::SameLine();
	if (ImGui::RadioButton("Move", &App->scene_intro->toolMode, (int)tool_mode::MOVE)) {
	
	} ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", &App->scene_intro->toolMode, (int)tool_mode::ROTATE)) {
	
	} ImGui::SameLine();
	if (ImGui::RadioButton("Scale", &App->scene_intro->toolMode, (int)tool_mode::SCALE)) {
	
	} ImGui::SameLine();
	if (ImGui::RadioButton("Rect", &App->scene_intro->toolMode, (int)tool_mode::RECT)) {
	
	} ImGui::SameLine();
	if (ImGui::RadioButton("Multi", &App->scene_intro->toolMode, (int)tool_mode::MULTI)) {
	
	}

	// Tool Handles
	float itemWidth = 50.f;
	ImGui::SameLine(400.f);
	ImGui::SetNextItemWidth(itemWidth);
	const char* handlePos[2] = { "Center", "Pivot" };
	if (ImGui::BeginCombo("##HandlePos", handlePos[App->scene_intro->handlePosition], ImGuiComboFlags_NoArrowButton)) // The second parameter is the label previewed before opening the combo.
	{
		for (int i = 0; i < IM_ARRAYSIZE(handlePos); i++)
		{
			bool is_selected = (App->scene_intro->handlePosition == i);
			if (ImGui::Selectable(handlePos[i], is_selected)) {
				App->scene_intro->handlePosition = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
		}
		ImGui::EndCombo();
	}
	HoverTip("Tool Handle Position", true);

	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth);
	const char* handleRot[2] = { "Local", "Global" };
	if (ImGui::BeginCombo("##HandleRot", handleRot[App->scene_intro->handleRotation], ImGuiComboFlags_NoArrowButton)) // The second parameter is the label previewed before opening the combo.
	{
		for (int i = 0; i < IM_ARRAYSIZE(handleRot); i++)
		{
			bool is_selected = (App->scene_intro->handleRotation == i);
			if (ImGui::Selectable(handleRot[i], is_selected)) {
				App->scene_intro->handleRotation = i;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
		}
		ImGui::EndCombo();
	}
	HoverTip("Tool Handle Rotation", true);

	// Scene Play
	ImVec2 buttonSize = { itemWidth, 20.f };		// IMPROVE: Image Buttons
	ImGui::SameLine(windowCenter - buttonSize.x * 1.7f);
	if (ImGui::Button("Play", buttonSize)) {

	}
	ImGui::SameLine(windowCenter - buttonSize.x * 0.5f);
	if (ImGui::Button("Pause", buttonSize)) {

	}
	ImGui::SameLine(windowCenter - buttonSize.x * -0.7f);
	if (ImGui::Button("Frame", buttonSize)) {

	}
}