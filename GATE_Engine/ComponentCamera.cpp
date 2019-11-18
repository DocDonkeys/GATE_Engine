#include "Globals.h"
#include "ComponentCamera.h"

#include "libs/MathGeoLib/include/Math/MathFunc.h"
#include "libs/MathGeoLib/include/Math/float3x3.h"
#include "libs/MathGeoLib/include/Math/float4x4.h"
#include "libs/MathGeoLib/include/Geometry/Plane.h"

ComponentCamera::ComponentCamera() : Component()
{
	type = COMPONENT_TYPE::CAMERA;

	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3::zero;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 1.0f;
	frustum.farPlaneDistance = 1000.0f;
	frustum.verticalFov = DegToRad(60.0f);
	SetAspectRatio(1.3f);
}

ComponentCamera::~ComponentCamera()
{}

void ComponentCamera::Enable()
{
	active = true;
}

void ComponentCamera::Disable()
{
	active = false;
}

float ComponentCamera::GetNearPlaneDist() const
{
	return frustum.nearPlaneDistance;
}

float ComponentCamera::GetFarPlaneDist() const
{
	return frustum.farPlaneDistance;
}

float ComponentCamera::GetFOV() const
{
	return RadToDeg(frustum.verticalFov);
}

float ComponentCamera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

void ComponentCamera::SetNearPlaneDist(float dist)
{
	if (dist > 0.0f && dist < frustum.farPlaneDistance)
	{
		frustum.nearPlaneDistance = dist;
		needsProjectionUpdate = true;
	}
}

void ComponentCamera::SetFarPlaneDist(float dist)
{
	if (dist > 0.0f && dist > frustum.nearPlaneDistance)
	{
		frustum.farPlaneDistance = dist;
		needsProjectionUpdate = true;
	}
}

void ComponentCamera::SetFOV(float fov)
{
	float aspect_ratio = frustum.AspectRatio();

	frustum.verticalFov = DegToRad(fov);
	SetAspectRatio(aspect_ratio);
}

void ComponentCamera::SetAspectRatio(float aspect_ratio)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
	needsProjectionUpdate = true;
}

void ComponentCamera::LookAt(const float3& position)
{
	float3 dir = position - frustum.pos;

	float3x3 m = float3x3::LookAt(frustum.front, dir.Normalized(), frustum.up, float3::unitY);

	frustum.front = m.MulDir(frustum.front).Normalized();
	frustum.up = m.MulDir(frustum.up).Normalized();
}

float4x4 ComponentCamera::GetOpenGLView() const
{
	return float4x4(frustum.ViewMatrix()).Transposed();
}

float4x4 ComponentCamera::GetOpenGLProjection() const
{
	return frustum.ProjectionMatrix().Transposed();
}

float4x4 ComponentCamera::GetViewMatrix() const
{
	return frustum.ViewMatrix();
}

float4x4 ComponentCamera::GetProjectionMatrix() const
{
	//return frustum.ProjectionMatrix();
	return float4x4::D3DPerspProjRH(frustum.nearPlaneDistance, frustum.farPlaneDistance, frustum.NearPlaneWidth(), frustum.NearPlaneHeight());
}

bool ComponentCamera::ContainsAABB(const AABB& refBox) const
{
	float3 corners[8];
	refBox.GetCornerPoints(corners);

	uint totalInside = 0;

	for (int i = 0; i < 6; ++i) {	// For each frustum plane
		int insideCount = 8;
		Plane& p = frustum.GetPlane(i);

		for (int j = 0; j < 8; ++j)	// For each AABB corner
			if (p.IsOnPositiveSide(corners[j]))	// Frustum plane normals point outside the Frustum volume, therefore "out = positive side"
				--insideCount;

		if (insideCount == 0)
			return false;	// FULLY OUT
		else
			totalInside += 1;
	}
	
	if (totalInside == 6)
		return true;	// FULLY IN
	else
		return true;	// PARTLY IN
}