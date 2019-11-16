#ifndef MODULECAMERA3D
#define MODULECAMERA3D

#include "Module.h"
#include "Globals.h"
#include "ComponentCamera.h"

#include "libs/MathGeoLib/include/Math/float4x4.h"

class GameObject;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	// Camera Orders
	void Move(const float3 &movement);
	void GoTo(const float3 &pos);
	void LookAt(const float3 &spot, float dist = 0.0f);
	//void LookFrom(const float3 &spot, const float3 &direction, float dist = 0.0f);
	void GoLook(const float3 &pos, const float3 &spot);
	void CenterToObject(GameObject* obj, float multiplier = 1.0f);

	// Gets
	float3 GetPosition() const;
	float* GetViewMatrix() const;
	float* GetProjectionMatrix() const;

private:
	// Input Checks
	void MoveCamera(float& movSpeed);
	void DragCamera(float delta_x, float delta_y);
	void Zoom(float delta_z);
	bool FirstPersonCamera(float& movSpeed);

	// Camera Rotations
	void Orbit(float motion_x, float motion_y);
	void Rotate(float motion_x, float motion_y);

	// Double Tap Checks
	void ProcessBoost(bool& boostType, float& currSpeed, void(ModuleCamera3D::*fPtr)(void));
	void MovBoostInput();
	void RotBoostInput();
	void FirstPersonBoostInput();
	void CheckStartBoost(int currKey, bool& boostType);

public:
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
	float3 reference;
	bool target = false;	// Flag for object being targeted
	ComponentCamera* editorCam = nullptr;

	// Dobule Tap
	Timer doubleTapTimer;
	uint doubleTapMsFrame = 1000;
	int lastKeyPressed = 0;

	bool boostingSpeed = false;
	bool boostingRot = false;
};

#endif	//MODULECAMERA3D