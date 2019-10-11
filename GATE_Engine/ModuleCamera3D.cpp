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
		float currSpeed = camMovSpeed * dt;

		// Mouse Button Controls
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
			|| App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT
			|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {

			float mouse_x = (float)-App->input->GetMouseXMotion() * camMouseSensivility;
			float mouse_y = (float)-App->input->GetMouseYMotion() * camMouseSensivility;

			if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT
				|| App->scene_intro->currMode == tool_mode::DRAG && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				DragCamera(newPos, mouse_x / 5.0f, mouse_y / 5.0f);	//Drag Camera
			}
			else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
				if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
					newPos += Z * mouse_y;	//Zoom Camera
				}
				else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {
					MouseRotate(rotate_type::AROUND, mouse_x, mouse_y);	//Rotate Camera around Reference
				}
			}
			else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
				MouseRotate(rotate_type::SELF, mouse_x, mouse_y);	//Rotate Camera around Self
			}
		}

		// Keyboard Controls & Mouse Wheel
		RotateCamera(camRotSpeed);
		MoveCamera(newPos, currSpeed);
		if (false) {	//CHANGE/FIX: Add controller variable and activation button for First Person Controls
			FirstPersonCamera(newPos, currSpeed, camRotSpeed, dt);	//Move & Rotate Camera with FirstPerson-like controls
		}

		// Apply changes and recalculate matrix
		Move(newPos);
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------

void ModuleCamera3D::MoveCamera(vec3& mov, float& speed)
{
	// Mouse Scroll
	mov -= Z * App->input->GetMouseZ();	// Mouse Scroll

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

void ModuleCamera3D::RotateCamera(float& rotSpeed)
{
	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_KP_4) == KEY_REPEAT) RotateHorizontal(rotSpeed);
	if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_REPEAT) RotateHorizontal(-rotSpeed);

	// Up/Down
	if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_REPEAT) RotateVertical(rotSpeed);
	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_REPEAT) RotateVertical(-rotSpeed);
}

void ModuleCamera3D::FirstPersonCamera(vec3& mov, float& movSpeed, float& rotSpeed, float& dt)
{
	// SELF_ROTATE
	RotateBegin(rotate_type::SELF);

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) RotateHorizontal(rotSpeed);
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) RotateHorizontal(-rotSpeed);

	// Up/Down
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) RotateVertical(rotSpeed);
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) RotateVertical(-rotSpeed);

	RotateFinish(rotate_type::SELF);
	// END SELF_ROTATE

	// TURBO
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		movSpeed = 20.0f * dt;	//CHANGE/FIX: Dehardcode

	// MOVE
	// Forward/Backwards
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) mov -= Z * movSpeed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) mov += Z * movSpeed;

	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) mov -= X * movSpeed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) mov += X * movSpeed;

	// Up/Down
	// Left/Right
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) mov -= Y * movSpeed;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) mov += Y * movSpeed;
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