#include "EditorInspector.h"
#include "ModuleEditor.h"
#include "EditorHierarchy.h"

#include "Application.h"
#include "ModuleSceneIntro.h"

#include "Component.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

// Memory Leak Detection
#include "MemLeaks.h"

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
		if (ImGui::Checkbox("Static", &go->staticObj)) {
			if (go->children.size() > 0) {
				show_static_modal = true;
				ImGui::OpenPopup("Changing Static State");
			}
			else {
				go->UpdateStaticStatus(go->staticObj, false);
			}
		} ImGui::SameLine();
		HoverTip("Editing the transform component of an object will remove the static status from himself and all of his children!");

		if (ImGui::BeginPopupModal("Changing Static State", &show_static_modal, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::PushTextWrapPos(300.f);
			ImGui::Text("Do you want to apply the static state switch to the object's children?");
			ImGui::PopTextWrapPos();
			ImGui::Spacing(); ImGui::SameLine(ImGui::GetWindowWidth() / 2.f - 90.f);
			if (ImGui::Button("Yes", { 90.f, 30.f })) {
				go->UpdateStaticStatus(go->staticObj, true);
				show_static_modal = false;
			} ImGui::SameLine();
			if (ImGui::Button("No", { 90.f, 30.f })) {
				go->UpdateStaticStatus(go->staticObj, false);
				show_static_modal = false;
			}

			ImGui::EndPopup();
		}

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

		ComponentCamera* camera = (ComponentCamera*)go->GetComponent(COMPONENT_TYPE::CAMERA);
		if (camera != nullptr)
		{
			DrawComponentCamera(camera);
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

		float3 pos, rot, scale;
		transform->GetLocalMat(pos, rot, scale);
		rot = RadToDeg(rot);

		ImGui::Columns(4, "TransformGrid"); // 4-ways, with border

		ImGui::Separator();
		if (ImGui::Button("Reset")) {
			pos = float3::zero;
			rot = float3::zero;
			scale = float3::one;
		}
		ImGui::NextColumn();
		ImGui::Text("X"); ImGui::NextColumn();
		ImGui::Text("Y"); ImGui::NextColumn();
		ImGui::Text("Z"); ImGui::NextColumn();
		ImGui::Separator();

		// Position
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Position"); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PX", &pos.x, 0.05f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PY", &pos.y, 0.05f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##PZ", &pos.z, 0.05f); ImGui::NextColumn();

		// Rotation
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Rotation"); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RX", &rot.x, 0.05f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RY", &rot.y, 0.05f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##RZ", &rot.z, 0.05f); ImGui::NextColumn();

		// Scale
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Scale   "); ImGui::NextColumn();
		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SX", &scale.x, 0.05f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SY", &scale.y, 0.05f); ImGui::NextColumn();

		ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##SZ", &scale.z, 0.05f);

		ImGui::Columns(1);
		ImGui::TreePop();

		// Data and Matrix Updating
		if (!App->input->GetMouseWrapping())
			if (transform->SetLocalMat(pos, DegToRad(rot), scale))
				transform->my_go->UpdateStaticStatus(false, true);
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
		HoverTip(mesh->mesh->path.c_str(), true);
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
			ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), material->active_texture->GetName());
			HoverTip(material->active_texture->GetFile(), true);
		}
		else
			ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "None");

		bool falseBool = false;
		/*ImGui::Image();*/ImGui::Checkbox("##placeholder1", &falseBool); ImGui::SameLine(); /*ImGui::DragBehavior();*/
		ImGui::Text("Albedo"); ImGui::SameLine();
		HoverTip("Albedo and the color palette are purely aesthetic currently, a UI design showcase of GATE's incoming features. To actually embed a texture on a material, simply Drag&Drop the file to the scene with the desired Game Object selected in the hierarchy.");
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

void EditorInspector::DrawComponentCamera(ComponentCamera* camera)
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (ImGui::TreeNodeEx("Camera", base_flags))
	{
		ImGui::Checkbox("Active", &camera->active); ImGui::SameLine();
		if (ImGui::Checkbox("Culling Testing", &camera->cullingTesting)) {
			if (camera->cullingTesting) {
				if (App->renderer3D->cullingTestTarget != nullptr)
					App->renderer3D->cullingTestTarget->cullingTesting = false;

				App->renderer3D->cullingTestTarget = camera;
			}
			else
				App->renderer3D->cullingTestTarget = nullptr;
		}

		ImGui::Separator();

		float nearPlane = camera->GetNearPlaneDist();
		float farPlane = camera->GetFarPlaneDist();
		float fov = camera->GetFOV();
		float aspectRatio = camera->GetAspectRatio();

		float width = ImGui::GetWindowWidth() / 3.f;

		ImGui::AlignTextToFramePadding(); ImGui::Text("Near Plane");
		ImGui::SameLine(130.f); ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##NearPlane", &nearPlane, 0.05f, 0.1f, farPlane);

		ImGui::AlignTextToFramePadding(); ImGui::Text("Far Plane");
		ImGui::SameLine(130.f); ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##FarPlane", &farPlane, 0.05f, nearPlane, 5000.f);

		ImGui::AlignTextToFramePadding(); ImGui::Text("Field of View");
		ImGui::SameLine(130.f); ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##FieldofView", &fov, 0.05f, 1.f, 179.f);

		ImGui::AlignTextToFramePadding(); ImGui::Text("Aspect Ratio");
		ImGui::SameLine(130.f); ImGui::SetNextItemWidth(width);
		ImGui::DragFloat("##AspectRatio", &aspectRatio, 0.05f, 0.1f, 10.f);

		if (!App->input->GetMouseWrapping()) {
			if (nearPlane != camera->GetNearPlaneDist())
				camera->SetNearPlaneDist(nearPlane);
			if (farPlane != camera->GetFarPlaneDist())
				camera->SetFarPlaneDist(farPlane);
			if (fov != camera->GetFOV())
				camera->SetFOV(fov);
			if (aspectRatio != camera->GetAspectRatio())
				camera->SetAspectRatio(aspectRatio);
		}

		ImGui::TreePop();
	}
	ImGui::Separator();
}