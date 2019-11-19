#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "GeometryLoader.h"

#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

ModuleCamera3D::ModuleCamera3D(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	activeCamera = editorCamera = new ComponentCamera();
}

ModuleCamera3D::~ModuleCamera3D()
{
	delete editorCamera;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	if (!App->editor->using_menu) {	//CHANGE/FIX: Should only work if Scene Window was the last window the user interacted with
		
		//float3 newPos(0, 0, 0);
		float currMovSpeed = camMovSpeed * dt;
		//float currRotSpeed = camRotSpeed * dt;

		// Double Tap Boosting
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {	// First person keys
				ProcessBoost(boostingSpeed, currMovSpeed, &ModuleCamera3D::FirstPersonBoostInput);
		}
		else {	// Regular keys
				ProcessBoost(boostingSpeed, currMovSpeed, &ModuleCamera3D::MovBoostInput);
				//ProcessBoost(boostingRot, currRotSpeed, &ModuleCamera3D::RotBoostInput);
		}

		float3 mouseInput = {
			(float)-App->input->GetMouseXMotion() * mouseSens * dt,
			(float)-App->input->GetMouseYMotion() * mouseSens * dt,
			(float)App->input->GetMouseZ() * scrollSens * dt };

		// Mouse Button Controls
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT
			|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT
			|| App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
			|| mouseInput.z != 0.f)
		{
			if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT
				||
				App->scene_intro->toolMode == (int)tool_mode::DRAG
				&& App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT
				&& App->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE) {	// Drag Camera
				DragCamera(mouseInput.x / 5.f, mouseInput.y / 5.f);
			}
			else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
				if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) Orbit(mouseInput.x, mouseInput.y);	// Rotate Camera around Reference
				else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) Zoom(-mouseInput.y);		// Zoom Camera
			}
			else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {	// Rotate Camera around Self
				Rotate(mouseInput.x, mouseInput.y);

				if (FirstPersonCamera(currMovSpeed)) {	// First Person Controls (if true == there's an input)
					camMovSpeed += mouseInput.z;	// Mouse Scroll: Increase/Decrease movement speed
					camMovSpeed = math::Clamp(camMovSpeed, 0.1f, maxMovSpeed);
				}
				else {
					Zoom(mouseInput.z);	// Mouse Scroll: Forward/Backwrads
				}
			}
		}

		// Regular Controls
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT) {	// If First Person disabled...
			MoveCamera(currMovSpeed);		// Arrow Key Controls
			Zoom(mouseInput.z);					// Mouse Scroll: Forward/Backwrads	// Mouse Scroll: Zoom
		}

		// Center camera to object
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {	// Focus camera to position from a certain dist and angle
			if (App->scene_intro->selected_go == nullptr) {
				LOG("[Warning] You must select an object before centering to it!")
			}
			else {
				CenterToObject(App->scene_intro->selected_go);
			}
		}
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------

// Input Checks
void ModuleCamera3D::MoveCamera(float& movSpeed)
{
	float3 mov(float3::zero);

	float3 right(editorCamera->frustum.WorldRight());
	float3 front(editorCamera->frustum.front);

	// Forward/Backwards
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) mov += front;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) mov -= front;

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) mov -= right;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) mov += right;

	// Up/Down
	//if (App->input->GetKey(SDL_SCANCODE_KP_7) == KEY_REPEAT) mov -= float3::unitY;
	//if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_REPEAT) mov += float3::unitY;

	if (!mov.Equals(float3::zero)) {
		editorCamera->frustum.Translate(mov * movSpeed);
	}
}

void ModuleCamera3D::DragCamera(float delta_x, float delta_y)
{
	float3 mov(float3::zero);

	mov += editorCamera->frustum.WorldRight() * delta_x;
	mov -= editorCamera->frustum.up * delta_y;

	editorCamera->frustum.Translate(mov * 100.f);
	reference += mov * 100.f;
}

void ModuleCamera3D::Zoom(float delta_z)
{
	editorCamera->frustum.pos += editorCamera->frustum.front * delta_z * scrollSens;
}

bool ModuleCamera3D::FirstPersonCamera(float& movSpeed)
{
	bool ret = false;
	float3 mov(float3::zero);

	float3 right(editorCamera->frustum.WorldRight());
	float3 front(editorCamera->frustum.front);

	// Boost speed if not already
	if (!boostingSpeed && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) movSpeed *= camMovMultiplier;

	// Forward/Backwards
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) mov += front;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) mov -= front;

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) mov -= right;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) mov += right;

	// Down/Up
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) mov -= float3::unitY;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) mov += float3::unitY;

	if (!mov.Equals(float3::zero)) {
		editorCamera->frustum.Translate(mov * movSpeed);
		ret = true;
	}

	return ret;
}

// -----------------------------------------------------------------

// Camera Rotations
void ModuleCamera3D::Orbit(float dx, float dy)
{
	float3 focus = editorCamera->frustum.pos - reference;

	Quat qy(editorCamera->frustum.up, dx);
	Quat qx(editorCamera->frustum.WorldRight(), dy);

	focus = qx.Transform(focus);
	focus = qy.Transform(focus);

	editorCamera->frustum.pos = focus + reference;

	LookAt(reference);
}

void ModuleCamera3D::Rotate(float dx, float dy)
{
	// X motion make the camera rotate in Y global axis
	if (dx != 0.f)
	{
		Quat q = Quat::RotateY(dx);
		editorCamera->frustum.front = q.Mul(editorCamera->frustum.front).Normalized();
		editorCamera->frustum.up = q.Mul(editorCamera->frustum.up).Normalized();
	}

	// Y motion makes the camera rotate in X local axis
	if (dy != 0.f)
	{
		Quat q = Quat::RotateAxisAngle(editorCamera->frustum.WorldRight(), dy);
		float3 new_up = q.Mul(editorCamera->frustum.up).Normalized();

		if (new_up.y > 0.0f)
		{
			editorCamera->frustum.up = new_up;
			editorCamera->frustum.front = q.Mul(editorCamera->frustum.front).Normalized();
		}
	}

	float3 dist = reference - editorCamera->frustum.pos;
	reference = editorCamera->frustum.pos + editorCamera->frustum.front * dist.Length();
}

// -----------------------------------------------------------------

// Double Tap Boosting
void ModuleCamera3D::ProcessBoost(bool& boostType, float& currSpeed, void(ModuleCamera3D::*fPtr)(void))
{
	if (boostType) {
		if (App->input->GetKey(lastKeyPressed) == KEY_UP) {
			boostType = false;
		}
		else {
			currSpeed *= camMovMultiplier;
		}
	}
	else {
		(this->*fPtr)();
	}
}

void ModuleCamera3D::MovBoostInput()	//IMPROVE: Add First-person camera condition and inputs
{
	int currKey = -1;

	// Forward/Backwards
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN) currKey = SDL_SCANCODE_DOWN;
	else if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN) currKey = SDL_SCANCODE_UP;

	// Left/Right
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN) currKey = SDL_SCANCODE_RIGHT;
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN) currKey = SDL_SCANCODE_LEFT;

	// Up/Down
	else if (App->input->GetKey(SDL_SCANCODE_KP_7) == KEY_DOWN) currKey = SDL_SCANCODE_KP_7;
	else if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN) currKey = SDL_SCANCODE_KP_1;

	CheckStartBoost(currKey, boostingSpeed);
}

void ModuleCamera3D::RotBoostInput()		//IMPROVE: Add First-person camera condition and inputs
{
	int currKey = -1;

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_DOWN) currKey = SDL_SCANCODE_KP_4;
	if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_DOWN) currKey = SDL_SCANCODE_KP_6;

	// Up/Down
	if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_DOWN) currKey = SDL_SCANCODE_KP_8;
	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN) currKey = SDL_SCANCODE_KP_2;

	CheckStartBoost(currKey, boostingRot);
}

void ModuleCamera3D::FirstPersonBoostInput()	//IMPROVE: Add First-person camera condition and inputs
{
	int currKey = -1;

	// Forward/Backwards
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) currKey = SDL_SCANCODE_W;
	else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) currKey = SDL_SCANCODE_S;

	// Left/Right
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) currKey = SDL_SCANCODE_A;
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) currKey = SDL_SCANCODE_D;

	// Down/Up
	else if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) currKey = SDL_SCANCODE_Q;
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) currKey = SDL_SCANCODE_E;

	CheckStartBoost(currKey, boostingSpeed);
}

void ModuleCamera3D::CheckStartBoost(int currKey, bool& boostType)
{
	if (currKey == lastKeyPressed && doubleTapTimer.Read() < doubleTapMsFrame) {
		boostType = true;
	}
	else if (currKey != -1) {
		lastKeyPressed = currKey;
		doubleTapTimer.Start();
	}
}

// -----------------------------------------------------------------

//Camera Orders
void ModuleCamera3D::Move(const float3 &mov)
{
	editorCamera->frustum.pos += mov;
	reference += mov;
}

void ModuleCamera3D::GoTo(const float3 &pos)
{
	float3 dist = pos - editorCamera->frustum.pos;
	editorCamera->frustum.pos = pos;
	reference = editorCamera->frustum.pos + dist;
}

void ModuleCamera3D::LookAt(const float3 &spot, float dist)
{
	reference = spot;
	editorCamera->LookAt(reference);

	if (dist > 0.f)
		editorCamera->frustum.pos = reference - editorCamera->frustum.front * dist;
}

void ModuleCamera3D::GoLook(const float3 &pos, const float3 &spot)	// Look at reference from a certain position
{
	GoTo(pos);
	LookAt(spot);
}

void ModuleCamera3D::CenterToObject(GameObject* obj, float multiplier)	//IMPROVE: This should not change the camera's angle
{
	if (obj != nullptr) {
		float dist = Length({ 10.0f, 10.0f, 10.0f });

		ComponentTransform* transform = (ComponentTransform*)obj->GetComponent(COMPONENT_TYPE::TRANSFORM);
		if (transform != nullptr)
			reference = transform->position;

		ComponentMesh* mesh = (ComponentMesh*)obj->GetComponent(COMPONENT_TYPE::MESH);
		if (mesh != nullptr)
			dist = Length(float3(mesh->mesh->size.x, mesh->mesh->size.y, mesh->mesh->size.z)) * Length(float3(transform->scale.x, transform->scale.y, transform->scale.z));

		LookAt(reference, dist);
	}
}

float3 ModuleCamera3D::GetPosition() const
{
	return activeCamera->frustum.pos;
}

float* ModuleCamera3D::GetOpenGLView() const
{
	return *activeCamera->GetOpenGLView().v;
}

float* ModuleCamera3D::GetOpenGLProjection() const
{
	return *activeCamera->GetOpenGLProjection().v;
}

bool* ModuleCamera3D::GetProjectionUpdateFlag() const
{
	return &activeCamera->needsProjectionUpdate;
}

ComponentCamera* ModuleCamera3D::GetActiveCamera() const
{
	return activeCamera;
}

bool ModuleCamera3D::ContainsAABB(const AABB& refBox) const
{
	return activeCamera->ContainsAABB(refBox);
}