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
	void RotateCamera(float& rotSpeed);
	void DragCamera(vec3& mov, float delta_x, float delta_y);
	void MouseRotate(rotate_type type, float delta_x, float delta_y);
	void FirstPersonCamera(vec3& mov, float& movSpeed, float& rotSpeed, float& dt);

	// Camera Rotations
	void RotateBegin(rotate_type type);
	void RotateFinish(rotate_type type);

	void RotateHorizontal(float angle);
	void RotateVertical(float angle);

	// Matrix Calc
	void CalculateViewMatrix();

public:
	vec3 X, Y, Z, Position, Reference;

	float camMovSpeed = 30.0f;			// Movement speed for keyboard inputs		//CHANGE/FIX: Save&Load
	float camRotSpeed = 1.0f;			// Rotation speed for keyboard inputs
	float camMouseSensivility = 0.25f;	// Multiplier for mouse input

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
	
};