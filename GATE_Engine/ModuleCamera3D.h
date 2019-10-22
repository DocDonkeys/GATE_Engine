#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	void GoLook(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void GoLook(const vec3 &Reference, float Distance, const vec3 &direction);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

private:
	enum class rotate_type {
		SELF,
		AROUND
	};

	// Input Checks
	void MoveCamera(vec3& mov, float& speed);
	void RotateCamera(rotate_type rotType, float& rotSpeed);
	void DragCamera(vec3& mov, float delta_x, float delta_y);
	void MouseRotate(rotate_type type, float delta_x, float delta_y);
	bool FirstPersonCamera(vec3& mov, float& movSpeed);

	// Double Tap Checks
	void ProcessBoost(bool& boostType, float& currSpeed, void(ModuleCamera3D::*fPtr)(void));
	void MovBoostInput();
	void RotBoostInput();
	void FirstPersonBoostInput();
	void CheckStartBoost(int currKey, bool& boostType);

	// Camera Rotations
	void RotateBegin(rotate_type type);
	void RotateFinish(rotate_type type);

	void RotateHorizontal(float angle);
	void RotateVertical(float angle);

	// Matrix Calc
	void CalculateViewMatrix();

public:
	vec3 X, Y, Z, Position, Reference;

	// Camera Movement
	float camMovSpeed;			// Movement speed for keyboard inputs
	float maxMovSpeed;			// Maximum movement speed
	float camMovMultiplier;		// Movement speed multiplier for percentage boosts
	float maxMovMultiplier;		// Maximum movement multiplier

	// Camera Rotation
	float camRotSpeed;			// Rotation speed for keyboard inputs
	float maxRotSpeed;			// Maximum rotation speed
	float camRotMultiplier;		// Rotation speed multiplier for percentage boosts
	float maxRotMultiplier;		// Maximum rotation multiplier

	// Mouse input multiplier
	float camMouseSens;		// Multiplier for mouse input
	float maxMouseSens;		// Maximum mouse sensibility

	float camDefaultMin = 0.1f;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;

	// Dobule Tap
	Timer doubleTapTimer;
	uint doubleTapMsFrame = 1000;
	int lastKeyPressed = 0;

	bool boostingSpeed = false;
	bool boostingRot = false;
};
