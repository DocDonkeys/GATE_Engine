#include "EditorInspector.h"
#include "ModuleEditor.h"
#include "EditorHierarchy.h"

#include "Application.h"
#include "ModuleSceneIntro.h"

#include "Component.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

EditorInspector::EditorInspector(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorInspector::Update()
{
	int focused_go = App->editor->editor_hierarchy->focus_node;
	if (focused_go > -1)
	{
		GameObject* go = App->scene_intro->game_objects[focused_go];
		//First show on editor the Gameobject class editables
		ImGui::Checkbox("",&go->active); //ImGui::SameLine();
		//ImGui::InputText((const char*)go->name.data(), input_buffer, IM_ARRAYSIZE(input_buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory);


		//For now we will only have 1 Component of each type, must move to arrays and create gameobject function to return all components of a type in an array
		ComponentTransform* transform = (ComponentTransform*)go->GetComponent(COMPONENT_TYPE::TRANSFORM);
		if (transform != nullptr)
		{
			DrawComponentTransform(transform);
		}

		ComponentMesh* mesh = (ComponentMesh*)go->GetComponent(COMPONENT_TYPE::MESH);
		if (mesh != nullptr)
		{
			DrawComponentMesh(mesh);
		}

		ComponentMaterial* material = (ComponentMaterial*)go->GetComponent(COMPONENT_TYPE::MATERIAL);
		if (material != nullptr)
		{
			DrawComponentMaterial(material);
		}
	}
}

void EditorInspector::DrawComponent(Component * c)
{
	switch (c->type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		DrawComponentTransform((ComponentTransform*)c);
		break;
	case COMPONENT_TYPE::MESH:
		DrawComponentMesh((ComponentMesh*)c);
		break;
	case COMPONENT_TYPE::MATERIAL:
		DrawComponentMaterial((ComponentMaterial*)c);
		break;
	default:
		break;
	}
}

void EditorInspector::DrawComponentTransform(ComponentTransform * transform)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx("Transform", base_flags))
	{
		float width = ImGui::GetWindowWidth() / 7.0f;

		ImGui::Columns(4, "TransformGrid"); // 4-ways, with border
		
		ImGui::Separator();
		ImGui::Text(""); ImGui::NextColumn();
		ImGui::Text("X"); ImGui::NextColumn();
		ImGui::Text("Y"); ImGui::NextColumn();
		ImGui::Text("Z"); ImGui::NextColumn();
		ImGui::Separator();

		// Position
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position"); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PX", &transform->position.x, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PY", &transform->position.y, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PZ", &transform->position.y, 0.005f); ImGui::NextColumn();

		// Rotation
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation"); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RX", &transform->rotation.x, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RY", &transform->rotation.y, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RZ", &transform->rotation.y, 0.005f); ImGui::NextColumn();

		// Scale
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Scale   "); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SX", &transform->scale.x, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SY", &transform->scale.y, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SZ", &transform->scale.y, 0.005f);

		ImGui::Columns(1);
		ImGui::TreePop();
	}

	ImGui::Separator();
}

void EditorInspector::DrawComponentMesh(ComponentMesh * mesh)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx("Mesh", base_flags))
	{
		ImGui::Text("File: "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 255.0f, 255.0f, 255.00f), mesh->filename.c_str());

		ImGui::TreePop();
	}
	ImGui::Separator();
}

void EditorInspector::DrawComponentMaterial(ComponentMaterial * material)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx("Material", base_flags))
	{
		ImGui::Spacing();

		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 4.0f);
		ImGui::SameLine(150);
		if (ImGui::BeginCombo("Shader", "Standard")) {

			ImGui::EndCombo();
		}

		ImGui::Spacing();
		ImGui::SameLine(150);

		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 4.0f);
		if (ImGui::BeginCombo("Rendering Mode", "Opaque")) {

			ImGui::EndCombo();
		}

		ImGui::Text("Main Maps");
		//ImGui::DragBehavior();
		bool falseBool = false;
		/*ImGui::Image();*/ImGui::Checkbox("##placeholder1", &falseBool); ImGui::SameLine();
		ImGui::Text("Albedo"); ImGui::SameLine(150);

		if (ImGui::ColorButton("Color", { material->color.x, material->color.y, material->color.z, material->color.w })) {
			ImGui::OpenPopup("Palette");
		}

		if (ImGui::BeginPopup("Palette")) {
			float col[4]{ material->color.x, material->color.y, material->color.z, material->color.w };
			ImGui::ColorPicker4("Color", col);

			material->color.x = col[0];
			material->color.y = col[1];
			material->color.z = col[2];
			material->color.w = col[3];

			ImGui::EndPopup();
		}

		ImGui::TreePop();
	}
	ImGui::Separator();
}
