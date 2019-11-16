#include "Globals.h"
#include "ComponentCamera.h"

#include "libs/MathGeoLib/include/Math/MathFunc.h"
#include "libs/MathGeoLib/include/Math/float3x3.h"
#include "libs/MathGeoLib/include/Math/float4x4.h"

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

	projection_changed = true;
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
		projection_changed = true;
	}
}

void ComponentCamera::SetFarPlaneDist(float dist)
{
	if (dist > 0.0f && dist > frustum.nearPlaneDistance)
	{
		frustum.farPlaneDistance = dist;
		projection_changed = true;
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
	// More about FOV: http://twgljs.org/examples/fov-checker.html
	// fieldOfViewX = 2 * atan(tan(fieldOfViewY * 0.5) * aspect)
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * aspect_ratio);
	projection_changed = true;
}

void ComponentCamera::LookAt(const float3& position)
{
	float3 dir = position - frustum.pos;

	float3x3 m = float3x3::LookAt(frustum.front, dir.Normalized(), frustum.up, float3::unitY);

	frustum.front = m.MulDir(frustum.front).Normalized();
	frustum.up = m.MulDir(frustum.up).Normalized();
}

float4x4 ComponentCamera::GetOpenGLViewMatrix() const
{
	float4x4 m;

	m = frustum.ViewMatrix();
	m.Transpose();

	return m;
}

float4x4 ComponentCamera::GetOpenGLProjectionMatrix() const
{
	float4x4 m;

	m = frustum.ProjectionMatrix();
	m.Transpose();

	return m;
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