#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Globals.h"
#include "Component.h"

#include "libs/MathGeoLib/include/Geometry/Frustum.h"
#include "libs/MathGeoLib/include/Geometry/AABB.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera();
	ComponentCamera(float fov, float aspectRatio = 1.3f, float nearPlane = 1.f, float farPlane = 1000.f);
	~ComponentCamera();

	void Enable() override;
	void Disable() override;

	void Update() override;
	void Draw() override;

public:
	// Orders
	void LookAt(const float3& position);

	// Get
	float GetNearPlaneDist() const;
	float GetFarPlaneDist() const;
	float GetFOV() const;
	float GetAspectRatio() const;

	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	float4x4 GetOpenGLView() const;
	float4x4 GetOpenGLProjection() const;

	// Set
	void SetNearPlaneDist(float dist);
	void SetFarPlaneDist(float dist);
	void SetFOV(float fov);
	void SetAspectRatio(float aspect_ratio);

	// Flags
	bool Intersects(const AABB& refBox) const;	// Frustum culling
	static bool Intersects(const Frustum& frustum, const AABB& refBox);	// Function for global use

public:
	Frustum frustum;
	bool needsProjectionUpdate = true;
};

#endif // __COMPONENT_CAMERA_H__
