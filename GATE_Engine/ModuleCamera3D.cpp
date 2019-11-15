#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "GeometryLoader.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

ModuleCamera3D::ModuleCamera3D(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 0.0f, 5.0f);
	reference = float3(0.0f, 0.0f, 0.0f);

	CalculateViewMatrix();
}

ModuleCamera3D::~ModuleCamera3D()
{}

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
		
		float3 newPos(0, 0, 0);
		float currMovSpeed = camMovSpeed * dt;
		float currRotSpeed = camRotSpeed;

		// Double Tap Boosting
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {	// First person keys
				ProcessBoost(boostingSpeed, currMovSpeed, &ModuleCamera3D::FirstPersonBoostInput);
		}
		else {	// Regular keys
				ProcessBoost(boostingSpeed, currMovSpeed, &ModuleCamera3D::MovBoostInput);
				ProcessBoost(boostingRot, currRotSpeed, &ModuleCamera3D::RotBoostInput);
		}

		// Mouse Button Controls
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
			|| App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT
			|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {	// If there's a mouse input...

			float mouse_x = (float)-App->input->GetMouseXMotion() * camMouseSens;
			float mouse_y = (float)-App->input->GetMouseYMotion() * camMouseSens;

			if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT
				|| App->scene_intro->toolMode == (int)tool_mode::DRAG && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {	// Drag Camera
				DragCamera(newPos, mouse_x / 5.0f, mouse_y / 5.0f);
			}
			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
				if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {	// Zoom Camera
					newPos += Z * mouse_y;
				}
				else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {	// Rotate Camera around Reference
					MouseRotate(rotate_type::AROUND, mouse_x, mouse_y);
				}
			}
			else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {	// Rotate Camera around Self
				MouseRotate(rotate_type::SELF, mouse_x, mouse_y);

				if (FirstPersonCamera(newPos, currMovSpeed)) {	// First Person Controls (if true == there's an input)
					camMovSpeed += App->input->GetMouseZ();	// Mouse Scroll: Increase/Decrease movement speed
					camMovSpeed = math::Clamp(camMovSpeed, 0.1f, maxMovSpeed);
				}
				else {
					newPos -= Z * App->input->GetMouseZ();	// Mouse Scroll: Forward/Backwrads
				}
			}
		}

		// Regular Controls
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT) {	// If First Person disabled...
			MoveCamera(newPos, currMovSpeed);		// Arrow Key Controls
			newPos -= Z * App->input->GetMouseZ();	// Mouse Scroll: Zoom

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
				RotateCamera(rotate_type::AROUND, currRotSpeed);	// Rotate around reference
			else
				RotateCamera(rotate_type::SELF, currRotSpeed);		// Rotate self
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

		// Apply changes and recalculate matrix
		Move(newPos);
	}
	
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------

void ModuleCamera3D::MoveCamera(float3& mov, float& speed)
{
	// Forward/Backwards
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) mov += Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) mov -= Z * speed;

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) mov += X * speed;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) mov -= X * speed;

	// Up/Down
	if (App->input->GetKey(SDL_SCANCODE_KP_7) == KEY_REPEAT) mov += Y * speed;
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_REPEAT) mov -= Y * speed;
}

void ModuleCamera3D::RotateCamera(rotate_type rotType, float& rotSpeed)
{
	int inverter = 1.0f;

	if (rotType == rotate_type::AROUND)
		inverter *= -1.0f;

	RotateBegin(rotType);

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_REPEAT) RotateHorizontal(rotSpeed * inverter);
	if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_REPEAT) RotateHorizontal(-rotSpeed * inverter);

	// Up/Down
	if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_REPEAT) RotateVertical(rotSpeed * inverter);
	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_REPEAT) RotateVertical(-rotSpeed * inverter);

	RotateFinish(rotType);
}

bool ModuleCamera3D::FirstPersonCamera(float3& mov, float& movSpeed)
{
	bool ret = false;

	// Boost speed if not already
	if (!boostingSpeed && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		movSpeed *= camMovMultiplier;
	}

	// Forward/Backwards
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		mov -= Z * movSpeed; ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		mov += Z * movSpeed; ret = true;
	}

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		mov -= X * movSpeed; ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		mov += X * movSpeed; ret = true;
	}

	// Down/Up
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) {
		mov -= Y * movSpeed; ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) {
		mov += Y * movSpeed; ret = true;
	}

	return ret;
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

void ModuleCamera3D::DragCamera(float3& mov, float delta_x, float delta_y)
{
	mov += X * delta_x;
	mov -= Y * delta_y;
}

void ModuleCamera3D::MouseRotate(rotate_type type, float delta_x, float delta_y)
{
	RotateBegin(type);

	if (delta_x != 0)
		RotateHorizontal(delta_x);

	if (delta_y != 0)
		RotateVertical(delta_y);

	RotateFinish(type);
}

// -----------------------------------------------------------------

//Camera Movement
void ModuleCamera3D::Move(const float3 &mov)
{
	position += mov;
	reference += mov;

	CalculateViewMatrix();
}

//Camera Rotation Preparations
void ModuleCamera3D::RotateBegin(rotate_type type)	// Used to setup for future rotations of a certain type
{
	switch (type) {
	case rotate_type::SELF:
		reference += position;
		break;
	case rotate_type::AROUND:
		position -= reference;
		break;
	}
}

void ModuleCamera3D::RotateFinish(rotate_type type)	// Used when all rotations are finished
{
	switch (type) {
	case rotate_type::SELF:
		reference = position - Z * Length(position);
		break;
	case rotate_type::AROUND:
		position = reference + Z * Length(position);
		break;
	}
}

// Camera Rotations
void ModuleCamera3D::RotateHorizontal(float angle)
{
	//X = rotate(X, angle, float3(0.0f, 1.0f, 0.0f));
	//Y = rotate(Y, angle, float3(0.0f, 1.0f, 0.0f));
	//Z = rotate(Z, angle, float3(0.0f, 1.0f, 0.0f));
}

void ModuleCamera3D::RotateVertical(float angle)
{
	//Y = rotate(Y, angle, X);
	//Z = rotate(Z, angle, X);

	if (Y.y < 0.0f)
	{
		Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
		Y = Cross(Z, X);
	}
}

// -----------------------------------------------------------------

// Camera fetch orders
void ModuleCamera3D::GoLook(const float3 &pos, const float3 &spot, bool rotateAroundReference)	// Look at reference from a certain position
{
	position = pos;
	reference = spot;

	Z = float3(pos - spot).Normalized();
	X = Cross(float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	Y = Cross(Z, X);

	if(!rotateAroundReference)
	{
		reference = position;
		position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void ModuleCamera3D::LookFrom(const float3 &spot, const float3 &Direction, float dist)	// Look at reference from a certain direction
{
	float3 unitDirection = Direction.Normalized();

	if (dist > 0.0f) {
		position = spot + unitDirection * dist;
	}
	else {
		position = spot + Direction;
	}
	
	reference = spot;

	Z = unitDirection;
	X = Cross(float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	Y = Cross(Z, X);

	CalculateViewMatrix();
}

void ModuleCamera3D::LookAt(const float3 &spot, float dist)	// Look at reference from current direction
{
	reference = spot;

	float3 unitDirection = float3(position - reference).Normalized();

	if (dist > 0.0f) {
		position = spot + unitDirection * dist;
	}

	Z = unitDirection;
	X = Cross(float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	Y = Cross(Z, X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return *viewMatrix.v;
}

void ModuleCamera3D::CalculateViewMatrix()
{
	viewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -Dot(X, position), -Dot(Y, position), -Dot(Z, position), 1.0f);
	viewMatrixInverse = viewMatrix.Inverted();
}

void ModuleCamera3D::CenterToObject(GameObject* obj, float multiplier)	//IMPROVE: This should not change the camera's angle
{
	if (obj != nullptr) {
		float3 pos = { 0.0f, 0.0f, 0.0f };
		float dist = length({ 10.0f, 10.0f, 10.0f });

		ComponentTransform* transform = (ComponentTransform*)obj->GetComponent(COMPONENT_TYPE::TRANSFORM);
		if (transform != nullptr)
			pos = transform->globalTrs.TranslatePart();

		ComponentMesh* mesh = (ComponentMesh*)obj->GetComponent(COMPONENT_TYPE::MESH);
		if (mesh != nullptr)
			dist = length({ mesh->mesh->size.x, mesh->mesh->size.y, mesh->mesh->size.z });

		LookFrom({ pos.x, pos.y, pos.z }, { 1.0f, 1.0f, 1.0f }, dist * multiplier);
	}
}