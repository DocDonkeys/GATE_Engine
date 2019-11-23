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

	ImVec2 buttonSize = { 50.f, 20.f };		// IMPROVE: Image Buttons

	// Tool Handles
	ImGui::SameLine(400.f);
	int& posHandle = App->scene_intro->handlePositionMode;
	if (ImGui::Button(handlePositionStr[posHandle], buttonSize)) {
		switch (posHandle) {
		case (int)handle_position::CENTER:
			posHandle = (int)handle_position::PIVOT;
			break;
		case (int)handle_position::PIVOT:
			posHandle = (int)handle_position::CENTER;
			break;
		}
	}
	HoverTip("Tool Handle Position", true);

	ImGui::SameLine();
	int& rotHandle = App->scene_intro->handleRotationMode;
	if (ImGui::Button(handleRotationStr[rotHandle], buttonSize)) {
		switch (rotHandle) {
		case (int)handle_rotation::LOCAL:
			rotHandle = (int)handle_rotation::GLOBAL;
			break;
		case (int)handle_rotation::GLOBAL:
			rotHandle = (int)handle_rotation::LOCAL;
			break;
		}
	}
	HoverTip("Tool Handle Rotation", true);

	ImGui::SameLine(); HoverTip("<--- Transform tools not implemented yet!");	//CHANGE/FIX: Remove this when implementation is done

	// Game Options
	ImGui::SameLine(windowCenter - buttonSize.x * 1.7f);
	if (App->scene_intro->playing) {
		if (ImGui::Button("Stop", buttonSize)) {
			App->scene_intro->playing = false;
			App->scene_intro->paused = false;
		}
	}
	else {
		if (ImGui::Button("Play", buttonSize)) {
			App->scene_intro->playing = true;
		}
	}
	
	ImGui::SameLine(windowCenter - buttonSize.x * 0.5f);
	if (App->scene_intro->paused) {
		if (ImGui::Button("Resume", buttonSize)) {
			App->scene_intro->paused = false;
		}
	}
	else {
		if (ImGui::Button("Pause", buttonSize)) {
			App->scene_intro->paused = true;
		}
	}

	ImGui::SameLine(windowCenter - buttonSize.x * -0.7f);
	if (ImGui::Button("Tick", buttonSize) && App->scene_intro->playing == true) {
		App->scene_intro->requestTick = true;
		App->scene_intro->paused = true;
	}

	// Game Speed
	ImGui::SameLine(windowCenter - buttonSize.x * -2.2f);
	ImGui::SetNextItemWidth(100.f);
	ImGui::SliderFloat("Speed", &App->scene_intro->game_speed, 0.1f, App->scene_intro->game_max_speed);

	ImGui::OpenPopupOnItemClick("Speed", 1);
	if (ImGui::BeginPopup("Speed")) {
		if (ImGui::Button("Reset", buttonSize)) {
			App->scene_intro->game_speed = 1.f;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}