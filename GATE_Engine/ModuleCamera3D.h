#ifndef MODULECAMERA3D
#define MODULECAMERA3D

#include "Module.h"
#include "Globals.h"

#include "libs/MathGeoLib/include/Math/float4x4.h"

class GameObject;
class ComponentCamera;

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
	void GoLook(const float3 &pos, const float3 &spot);
	void CenterToObject(GameObject* obj, float multiplier = 1.0f);

	// Gets
	float3 GetPosition() const;
	float* GetOpenGLView() const;
	float* GetOpenGLProjection() const;
	bool* GetProjectionUpdateFlag() const;
	ComponentCamera* GetActiveCamera() const;
	const Frustum& GetActiveFrustum() const;

	bool Intersects(const AABB& refBox) const;

private:
	// Mouse picking
	GameObject* MousePick(float3* hit_point = nullptr) const;

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

	// Mouse input multiplier
	float mouseSens;
	float maxMouseSens;

	float scrollSens;
	float maxScrollSens;

	float camDefaultMin = 0.1f;

private:
	float3 reference;
	bool target = false;	// Flag for object being targeted
	ComponentCamera* editorCamera = nullptr;	// CHANGE/FIX: Save&Load of camera data
	ComponentCamera* activeCamera = nullptr;

	// Dobule Tap
	Timer doubleTapTimer;
	uint doubleTapMsFrame = 1000;
	int lastKeyPressed = 0;

	bool boostingSpeed = false;
	bool boostingRot = false;
};

#endif	//MODULECAMERA3D