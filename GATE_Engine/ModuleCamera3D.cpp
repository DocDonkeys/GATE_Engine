#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

#include "libs/MathGeoLib/include/Math/MathFunc.h"
#include "libs/MathGeoLib/include/Math/float3x3.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

ModuleCamera3D::ModuleCamera3D(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	CalculateViewMatrix();

	transform = transform.FromTRS(float3(0.0f, 0.0f, 5.0f), float4x4(float4x4::identity), float3(float3::one));
	reference = float3(0.0f, 0.0f, 0.0f);
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
#include "GeometryLoader.h"
#include "Mesh.h"
// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	if (!App->editor->using_menu) {

		float3 newPosition = transform.TranslatePart();

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
				DragCamera(newPosition, mouse_x / 5.0f, mouse_y / 5.0f);
			}
			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
				if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {	// Zoom Camera
					newPosition += transform.Col3(3) * mouse_y;
				}
				else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {	// Rotate Camera around Reference
					MouseRotate(rotate_type::AROUND, mouse_x, mouse_y);
				}
			}
			else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {	// Rotate Camera around Self
				MouseRotate(rotate_type::SELF, mouse_x, mouse_y);

				if (FirstPersonCamera(newPosition, currMovSpeed)) {	// First Person Controls (if true == there's an input)
					camMovSpeed += App->input->GetMouseZ();	// Mouse Scroll: Increase/Decrease movement speed
					camMovSpeed = Clamp(camMovSpeed, 0.1f, maxMovSpeed);
				}
				else {
					newPosition -= transform.Col3(3) * App->input->GetMouseZ();	// Mouse Scroll: Forward/Backwrads
				}
			}
		}

		// Regular Controls
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT) {	// If First Person disabled...
			MoveCamera(newPosition, currMovSpeed);		// Arrow Key Controls
			newPosition -= transform.Col3(3) * App->input->GetMouseZ();	// Mouse Scroll: Zoom

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
				RotateCamera(rotate_type::AROUND, currRotSpeed);	// Rotate around reference
			else
				RotateCamera(rotate_type::SELF, currRotSpeed);		// Rotate self
		}

		// Center camera to object
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {	// Focus camera to position from a certain distance and angle
			if (App->scene_intro->selected_go == nullptr) {
				LOG("[Warning] You must select an object before centering to it!")
			}
			else {
				CenterToObject(App->scene_intro->selected_go);
			}
		}

		// Apply changes and recalculate matrix
		MoveTo(newPosition);
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------

void ModuleCamera3D::MoveCamera(float3& mov, float& speed)
{
	// Forward/Backwards
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) mov += transform.WorldZ() * speed;
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) mov -= transform.WorldZ() * speed;

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) mov += transform.WorldX() * speed;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) mov -= transform.WorldX() * speed;

	// Up/Down
	if (App->input->GetKey(SDL_SCANCODE_KP_7) == KEY_REPEAT) mov += transform.WorldY() * speed;
	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_REPEAT) mov -= transform.WorldY() * speed;
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
		mov -= transform.WorldZ() * movSpeed; ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		mov += transform.WorldZ() * movSpeed; ret = true;
	}

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		mov -= transform.WorldX() * movSpeed; ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		mov += transform.WorldX() * movSpeed; ret = true;
	}

	// Down/Up
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) {
		mov -= transform.WorldY() * movSpeed; ret = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) {
		mov += transform.WorldY() * movSpeed; ret = true;
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
	mov += transform.WorldX() * delta_x;
	mov -= transform.WorldY() * delta_y;
}

void ModuleCamera3D::MouseRotate(rotate_type type, float delta_x, float delta_y)
{
	RotateBegin(type);

	transform.RotateX(delta_x);
	transform.RotateY(delta_x);

	/*if (delta_x != 0)
		RotateHorizontal(delta_x);

	if (delta_y != 0)
		RotateVertical(delta_y);*/

	RotateFinish(type);
}

// -----------------------------------------------------------------

//Camera Movement
void ModuleCamera3D::Move(const float3 &Movement)
{
	transform.SetTranslatePart(float3(transform.TranslatePart() + Movement));
	reference += Movement;

	CalculateViewMatrix();
}

void ModuleCamera3D::MoveTo(const float3 &position)
{
	reference += position - transform.TranslatePart();
	transform.SetTranslatePart(position);

	CalculateViewMatrix();
}

//Camera Rotation Preparations
void ModuleCamera3D::RotateBegin(rotate_type type)	// Used to setup for future rotations of a certain type
{
	switch (type) {
	case rotate_type::SELF:
		reference += transform.TranslatePart();
		break;
	case rotate_type::AROUND:
		transform.SetTranslatePart(float3(transform.TranslatePart() - reference));
		break;
	}
}

void ModuleCamera3D::RotateFinish(rotate_type type)	// Used when all rotations are finished
{
	switch (type) {
	case rotate_type::SELF:
		reference = transform.TranslatePart() - transform.WorldZ() * Length(transform.TranslatePart());
		break;
	case rotate_type::AROUND:
		transform.SetTranslatePart(float3(reference + transform.WorldZ() * Length(transform.TranslatePart())));
		break;
	}
}

// Camera Rotations
void ModuleCamera3D::RotateHorizontal(float angle)
{
	transform.RotateX(angle);
	//transform.RotateY(angle, float3(0.0f, 1.0f, 0.0f));
	//transform.RotateZ(angle, float3(0.0f, 1.0f, 0.0f));

	//X = rotate(X, angle, float3(0.0f, 1.0f, 0.0f));
	//Y = rotate(Y, angle, float3(0.0f, 1.0f, 0.0f));
	//Z = rotate(Z, angle, float3(0.0f, 1.0f, 0.0f));
}

void ModuleCamera3D::RotateVertical(float angle)
{
	transform.RotateY(angle, transform.WorldX());
	transform.RotateZ(angle, transform.WorldX());

	if (transform.WorldY().y < 0.0f)
	{
		transform.SetCol3(3, float3(0.0f, transform.WorldZ().y > 0.0f ? 1.0f : -1.0f, 0.0f));
		transform.SetCol3(2, Cross(transform.WorldZ(), transform.WorldX()));
	}

	//Y = rotate(Y, angle, X);
	//Z = rotate(Z, angle, X);

	/*if (Y.y < 0.0f)
	{
		Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
		Y = cross(Z, X);
	}*/
}

// -----------------------------------------------------------------

// Camera fetch orders
//void ModuleCamera3D::GoLook(const float3 &position, const float3 &spot, bool rotateAroundReference)	// Look at reference from a certain position
//{
//	
//	//------------
//
//	this->Position = Position;
//	this->Reference = Spot;
//
//	Z = normalize(position - spot);
//	X = normalize(cross(float3(0.0f, 1.0f, 0.0f), Z));
//	Y = cross(Z, X);
//	
//	if(!rotateAroundReference)
//	{
//		this->Reference = this->Position;
//		this->Position += Z * 0.05f;
//	}
//
//	CalculateViewMatrix();
//}

void ModuleCamera3D::LookFrom(const float3& spot, const float3& direction, float distance)	// Look at reference from a certain direction
{
	reference = spot;
	float3x3 rot = float3x3(float3(Cross(float3(0.0f, 1.0f, 0.0f), transform.WorldZ())).Normalized(), Cross(transform.WorldZ(), transform.WorldX()), direction.Normalized());

	float3 vec = direction;
	if (distance > 0.f)
		vec = vec.Normalized() * distance;

	transform = transform.FromTRS(spot + vec, rot, transform.GetScale());

	//-------------

	/*float3 unitDirection = normalize(Direction);

	if (Distance > 0.0f) {
		Position = Spot + unitDirection * Distance;
	}
	else {
		Position = Spot + Direction;
	}
	
	Reference = Spot;

	Z = unitDirection;
	X = normalize(cross(float3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);*/

	CalculateViewMatrix();
}

void ModuleCamera3D::LookAt(const float3 &spot, float distance)	// Look at reference from current direction
{
	reference = spot;
	float3 direction = float3(spot - transform.TranslatePart());
	float3x3 rot = float3x3(float3(Cross(float3(0.0f, 1.0f, 0.0f), transform.WorldZ())).Normalized(), Cross(transform.WorldZ(), transform.WorldX()), direction.Normalized());
	
	if (distance > 0.f) {
		direction = direction.Normalized() * distance;
		transform.FromTRS(spot + direction, rot, transform.GetScale());
	}
	else {
		transform.SetRotatePart(rot);
	}

	// -----------------

	/*reference = spot;

	float3 unitDirection = normalize(Position - reference);

	if (distance > 0.0f) {
		Position = spot + unitDirection * distance;
	}

	Z = unitDirection;
	X = normalize(cross(float3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);*/

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	viewMatrix = transform;
	viewMatrix.SetRow3(4, float3(-Dot(transform.WorldX(), transform.TranslatePart()), -Dot(transform.WorldY(), transform.TranslatePart()), -Dot(transform.WorldZ(), transform.TranslatePart())));

	//ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
}

void ModuleCamera3D::CenterToObject(GameObject* obj, float multiplier)	//IMPROVE: This should not change the camera's angle
{
	if (obj != nullptr) {
		float3 pos = { 0.0f, 0.0f, 0.0f };
		float dist = length({ 10.0f, 10.0f, 10.0f });

		ComponentTransform* transform = (ComponentTransform*)obj->GetComponent(COMPONENT_TYPE::TRANSFORM);
		if (transform != nullptr)
			pos = { transform->position.x, transform->position.y, transform->position.z };

		ComponentMesh* mesh = (ComponentMesh*)obj->GetComponent(COMPONENT_TYPE::MESH);
		if (mesh != nullptr)
			dist = length({ mesh->mesh->size.x, mesh->mesh->size.y, mesh->mesh->size.z });

		LookFrom(pos, { 1.0f, 1.0f, 1.0f }, dist * multiplier);
	}
}

float* ModuleCamera3D::GetViewMatrix()
{
	float* temp[16] = { &viewMatrix.v[0][0], &viewMatrix.v[0][1], &viewMatrix.v[0][2], &viewMatrix.v[0][3],
						&viewMatrix.v[1][0], &viewMatrix.v[1][1], &viewMatrix.v[1][2], &viewMatrix.v[1][3],
						&viewMatrix.v[2][0], &viewMatrix.v[2][1], &viewMatrix.v[2][2], &viewMatrix.v[2][3],
						&viewMatrix.v[3][0], &viewMatrix.v[3][1], &viewMatrix.v[3][2], &viewMatrix.v[3][3] };

	return *temp;
}