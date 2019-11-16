#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Globals.h"
#include "Component.h"

#include "libs/MathGeoLib/include/Geometry/Frustum.h"

class ComponentCamera : public Component
{
public:
	ComponentCamera();
	~ComponentCamera();

	void Enable() override;
	void Disable() override;

public:
	float GetNearPlaneDist() const;
	float GetFarPlaneDist() const;
	float GetFOV() const;
	float GetAspectRatio() const;

	void SetNearPlaneDist(float dist);
	void SetFarPlaneDist(float dist);
	void SetFOV(float fov);
	void SetAspectRatio(float aspect_ratio);

	void LookAt(const float3& position);

	float4x4 GetOpenGLViewMatrix() const;
	float4x4 GetOpenGLProjectionMatrix() const;

	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;

public:
	Frustum frustum;
	bool projection_changed = false;
};

#endif // __COMPONENT_CAMERA_H__
