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
	GameObject* go = App->scene_intro->selected_go;
	if (go != nullptr)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Checkbox("Active", &go->active); ImGui::SameLine();

		//if (!startedEditing)
		strcpy(objNameBuffer, go->name.c_str());

		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3.0f);
		if (ImGui::InputText("##objectName", objNameBuffer, IM_ARRAYSIZE(objNameBuffer)) && !startedEditing) {	//IMPROVE: It feels that this can be done better, memory is copied every frame and doesn't need to be
				//startedEditing = true;
		}
		//else if (startedEditing && App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		//	bool repeated = false;

		//	for (int i = 0; i < App->scene_intro->game_objects.size(); i++) {
		//		if (App->scene_intro->game_objects[i] != go && App->scene_intro->game_objects[i]->name == objNameBuffer) {
		//			LOG("A different GameObject is already using this name!");
		//			repeated = true;
		//		}
		//	}

		//	if (!repeated)
		go->name.assign(objNameBuffer);

		//	strcpy(objNameBuffer, "");
		//	startedEditing = false;
		//}

		//ImGui::SameLine();
		//ImGui::TextDisabled("(?)");	//CHANGE/FIX: Make HelpTip(const char*) function.
		//if (ImGui::IsItemHovered())
		//{
		//	ImGui::BeginTooltip();
		//	ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		//	ImGui::TextUnformatted("Press Enter to confirm your name change.");
		//	ImGui::PopTextWrapPos();
		//	ImGui::EndTooltip();
		//}
		ImGui::SameLine();
		ImGui::Checkbox("Static", &go->staticObj);

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Tag"); ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3.0f);
		if (ImGui::BeginCombo("##Tag", "Untagged")) {

			ImGui::EndCombo();
		}
		ImGui::SameLine();

		ImGui::Text("Layer"); ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3.0f);
		if (ImGui::BeginCombo("##Layer", "Default")) {

			ImGui::EndCombo();
		}

		ImGui::Separator();

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
			ComponentMaterial* material = (ComponentMaterial*)go->GetComponent(COMPONENT_TYPE::MATERIAL);
			if (material != nullptr)
			{
				DrawComponentMaterial(material);
			}
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

		float3 newPosition = transform->position;
		float3 newEulerRotation = transform->eulerRotation;
		float3 newScale = transform->scale;

		ImGui::Columns(4, "TransformGrid"); // 4-ways, with border

		ImGui::Separator();
		ImGui::Checkbox("Active", &transform->active); ImGui::NextColumn();
		ImGui::Text("X"); ImGui::NextColumn();
		ImGui::Text("Y"); ImGui::NextColumn();
		ImGui::Text("Z"); ImGui::NextColumn();
		ImGui::Separator();

		// Position
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position"); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PX", &newPosition.x, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PY", &newPosition.y, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PZ", &newPosition.z, 0.005f); ImGui::NextColumn();

		// Rotation
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation"); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RX", &newEulerRotation.x, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RY", &newEulerRotation.y, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RZ", &newEulerRotation.z, 0.005f); ImGui::NextColumn();

		// Scale
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Scale   "); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SX", &newScale.x, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SY", &newScale.y, 0.005f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SZ", &newScale.z, 0.005f);

		ImGui::Columns(1);
		ImGui::TreePop();

		// Data and Matrix Updating
		transform->UpdateValues(newPosition, newEulerRotation, newScale);
	}

	ImGui::Separator();
}

void EditorInspector::DrawComponentMesh(ComponentMesh * mesh)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx("Mesh", base_flags))
	{
		ImGui::Columns(2, "MeshGird"); // 2-ways, with border
		
		ImGui::Separator();
		ImGui::Checkbox("Active", &mesh->active); ImGui::NextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("File:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 255.0f, 255.0f, 255.00f), mesh->mesh->filename.c_str());
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(mesh->mesh->path.c_str());
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::NextColumn();
		ImGui::Separator();

		ImGui::Text("Draw:");
		ImGui::Checkbox("Vertex Normals", &mesh->debug_vertex_normals);
		ImGui::Checkbox("Face Normals", &mesh->debug_face_normals);
		ImGui::Text("Length:");  ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 7.0f);
		if (ImGui::DragFloat("##NL", &mesh->mesh->normals_length, 0.005f, 0.1f, 999.0f))
		{
			mesh->mesh->ChangeNormalsLength(mesh->mesh->normals_length);
		}

		ImGui::NextColumn();
		ImGui::Text("Indexes:"); ImGui::SameLine(); ImGui::Text("%u", mesh->mesh->num_index);
		ImGui::Text("Normals:"); ImGui::SameLine(); ImGui::Text("%u", mesh->mesh->num_normals);
		ImGui::Text("Vertexs:"); ImGui::SameLine(); ImGui::Text("%u", mesh->mesh->num_vertex);
		ImGui::Text("Faces:"); ImGui::SameLine(); ImGui::Text("%u", mesh->mesh->num_polys);
		ImGui::Text("Tex Coords:"); ImGui::SameLine(); ImGui::Text("%u", mesh->mesh->num_tex_coords);

		ImGui::Columns(1);
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

		ImGui::Checkbox("Active", &material->active); ImGui::SameLine();

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

		ImGui::AlignTextToFramePadding();
		ImGui::Text("File:"); ImGui::SameLine();
		if (material->active_texture != nullptr) {
			ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), material->active_texture->filename.c_str());
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(material->active_texture->path.c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
		else
			ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "None");

		bool falseBool = false;
		/*ImGui::Image();*/ImGui::Checkbox("##placeholder1", &falseBool); ImGui::SameLine(); /*ImGui::DragBehavior();*/
		ImGui::Text("Albedo"); ImGui::SameLine();

		ImGui::TextDisabled("(?)");	//CHANGE/FIX: Make HelpTip(const char*) function.
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted("Albedo and the color palette are purely aesthetic currently, a UI design showcase of GATE's incoming features. To actually embed a texture on a material, simply Drag&Drop the file to the scene with the desired Game Object selected in the hierarchy.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::SameLine(150);

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

		ImGui::Checkbox("Use Default Texture", &material->use_default_texture);

		ImGui::TreePop();
	}
	ImGui::Separator();
}
