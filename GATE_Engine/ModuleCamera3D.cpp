#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

ModuleCamera3D::ModuleCamera3D(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	App->ConsoleLOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	App->ConsoleLOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	if (true) {	//CHANGE/FIX: Should only work if Scene Window was the last window the user interacted with
		
		vec3 newPos(0, 0, 0);
		float currMovSpeed = camMovSpeed * dt;	//CHANGE/FIX: Multiplier should only trigger when double tapping a movement key
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

			float mouse_x = (float)-App->input->GetMouseXMotion() * camMouseSensivility;
			float mouse_y = (float)-App->input->GetMouseYMotion() * camMouseSensivility;

			if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT
				|| App->scene_intro->currMode == tool_mode::DRAG && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {	// Drag Camera
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
					camMovSpeed = CLAMP(camMovSpeed, camMaxMovSpeed, 0.1f);
				}
				else {
					newPos -= Z * App->input->GetMouseZ();	// Mouse Scroll: Forward/Backwrads
				}
			}
		}

		// Regular Controls
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT) {
			MoveCamera(newPos, currMovSpeed);		// Arrow Key Controls
			newPos -= Z * App->input->GetMouseZ();	// Mouse Scroll: Zoom

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
				RotateCamera(rotate_type::AROUND, currRotSpeed);	// Rotate around reference
			else
				RotateCamera(rotate_type::SELF, currRotSpeed);		// Rotate self
		}

		// Apply changes and recalculate matrix
		Move(newPos);
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------

void ModuleCamera3D::MoveCamera(vec3& mov, float& speed)
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

bool ModuleCamera3D::FirstPersonCamera(vec3& mov, float& movSpeed)
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

void ModuleCamera3D::DragCamera(vec3& mov, float delta_x, float delta_y)
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
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

//Camera Rotation Preparations
void ModuleCamera3D::RotateBegin(rotate_type type)	// Used to setup for future rotations of a certain type
{
	switch (type) {
	case rotate_type::SELF:
		Reference += Position;
		break;
	case rotate_type::AROUND:
		Position -= Reference;
		break;
	}
}

void ModuleCamera3D::RotateFinish(rotate_type type)	// Used when all rotations are finished
{
	switch (type) {
	case rotate_type::SELF:
		Reference = Position - Z * length(Position);
		break;
	case rotate_type::AROUND:
		Position = Reference + Z * length(Position);
		break;
	}
}

// Camera Rotations
void ModuleCamera3D::RotateHorizontal(float angle)
{
	X = rotate(X, angle, vec3(0.0f, 1.0f, 0.0f));
	Y = rotate(Y, angle, vec3(0.0f, 1.0f, 0.0f));
	Z = rotate(Z, angle, vec3(0.0f, 1.0f, 0.0f));
}

void ModuleCamera3D::RotateVertical(float angle)
{
	Y = rotate(Y, angle, X);
	Z = rotate(Z, angle, X);

	if (Y.y < 0.0f)
	{
		Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
		Y = cross(Z, X);
	}
}

// -----------------------------------------------------------------

// Camera fetch orders
void ModuleCamera3D::GoLook(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)	// Changes camera position, reference, and can be rotated around it
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void ModuleCamera3D::LookAt( const vec3 &Spot)	// Rotates camera to position and sets is as camera reference
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}