#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneIntro.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "libs/MathGeoLib/include/Math/MathFunc.h"
#include "libs/MathGeoLib/include/Math/float3x3.h"
#include "libs/MathGeoLib/include/Math/float4x4.h"
#include "libs/MathGeoLib/include/Geometry/Plane.h"

#include "libs/SDL/include/SDL_opengl.h"

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

ComponentCamera::ComponentCamera(float fov, float aspectRatio, float nearPlane, float farPlane) : Component()
{
	type = COMPONENT_TYPE::CAMERA;

	frustum.type = FrustumType::PerspectiveFrustum;

	frustum.pos = float3::zero;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = nearPlane;
	frustum.farPlaneDistance = farPlane;
	frustum.verticalFov = DegToRad(fov);
	SetAspectRatio(aspectRatio);
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

void ComponentCamera::Update(float realDT)
{
	ComponentTransform* trs = (ComponentTransform*)my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	frustum.pos = trs->GetGlobalMat().TranslatePart();
	frustum.front = trs->GetGlobalMat().WorldZ();
	frustum.up = trs->GetGlobalMat().WorldY();
}

void ComponentCamera::Draw()
{
	if (!App->renderer3D->drawObjAABB) {	// If Game object has size 0, draw a "gizmo" square instead
		my_go->DrawAABB(my_go->aabb, float3(1.f, 0.f, 1.f));
	}

	if (cullingTesting || App->scene_intro->selected_go == my_go) {
		DrawFrustum();
	}
}

void ComponentCamera::DrawFrustum()
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	float3 corners[8];
	frustum.GetCornerPoints(corners);

	// Sides
	glColor3f(1.f, 0.f, 1.f);

	// Left Face
	glVertex3f(corners[0].x, corners[0].y, corners[0].z);
	glVertex3f(corners[1].x, corners[1].y, corners[1].z);

	glVertex3f(corners[2].x, corners[2].y, corners[2].z);
	glVertex3f(corners[3].x, corners[3].y, corners[3].z);

	// Right Face
	glVertex3f(corners[4].x, corners[4].y, corners[4].z);
	glVertex3f(corners[5].x, corners[5].y, corners[5].z);

	glVertex3f(corners[6].x, corners[6].y, corners[6].z);
	glVertex3f(corners[7].x, corners[7].y, corners[7].z);

	// Near-Far
	glColor3f(1.f, 0.f, 0.f);

	// Near Plane
	glVertex3f(corners[0].x, corners[0].y, corners[0].z);
	glVertex3f(corners[4].x, corners[4].y, corners[4].z);

	glVertex3f(corners[2].x, corners[2].y, corners[2].z);
	glVertex3f(corners[6].x, corners[6].y, corners[6].z);

	glVertex3f(corners[0].x, corners[0].y, corners[0].z);
	glVertex3f(corners[2].x, corners[2].y, corners[2].z);

	glVertex3f(corners[4].x, corners[4].y, corners[4].z);
	glVertex3f(corners[6].x, corners[6].y, corners[6].z);

	// Far Plane
	glVertex3f(corners[1].x, corners[1].y, corners[1].z);
	glVertex3f(corners[5].x, corners[5].y, corners[5].z);

	glVertex3f(corners[3].x, corners[3].y, corners[3].z);
	glVertex3f(corners[7].x, corners[7].y, corners[7].z);

	glVertex3f(corners[1].x, corners[1].y, corners[1].z);
	glVertex3f(corners[3].x, corners[3].y, corners[3].z);

	glVertex3f(corners[5].x, corners[5].y, corners[5].z);
	glVertex3f(corners[7].x, corners[7].y, corners[7].z);

	glColor3f(1, 1, 1);
	glEnd();
}

void ComponentCamera::Save(json & file)
{
	file["UID"] = this->UID;
	file["Active"] = this->active;

	file["Fov"] = std::to_string(frustum.verticalFov);
	file["Aspect ratio"] = std::to_string(GetAspectRatio());
	file["Near Plane"] = std::to_string(frustum.nearPlaneDistance);
	file["Far Plane"] = std::to_string(frustum.farPlaneDistance);

	file["Frustum PosX"] = std::to_string(frustum.pos.x);
	file["Frustum PosY"] = std::to_string(frustum.pos.y);
	file["Frustum PosZ"] = std::to_string(frustum.pos.z);

	file["Frustum FrontX"] = std::to_string(frustum.front.x);
	file["Frustum FrontY"]= std::to_string(frustum.front.y);
	file["Frustum FrontZ"] = std::to_string(frustum.front.z);

	file["Frustum UpX"] = std::to_string(frustum.up.x);
	file["Frustum UpY"] = std::to_string(frustum.up.y);
	file["Frustum UpZ"] = std::to_string(frustum.up.z);
}

void ComponentCamera::Load(json & file)
{

	std::string Fov = file["Fov"];
	std::string Aspect_Ratio = file["Aspect ratio"];
	std::string Near_plane = file["Near Plane"];
	std::string Far_plane = file["Far Plane"];

	float fov = std::stof(Fov);
	float aspect_ratio = std::stof(Aspect_Ratio);
	float near_plane = std::stof(Near_plane);
	float far_plane = std::stof(Far_plane);

	float3 pos, front, up;

	std::string PosX = file["Frustum PosX"];
	std::string PosY = file["Frustum PosY"];
	std::string PosZ = file["Frustum PosZ"];

	std::string FrontX = file["Frustum PosX"];
	std::string FrontY = file["Frustum PosY"];
	std::string FrontZ = file["Frustum PosZ"];

	std::string UpX = file["Frustum UpX"];
	std::string UpY = file["Frustum UpY"];
	std::string UpZ = file["Frustum UpZ"];

	pos.x = std::stof(PosX);
	pos.y = std::stof(PosY);
	pos.z = std::stof(PosZ);

	front.x = std::stof(FrontX);
	front.y = std::stof(FrontY);
	front.z = std::stof(FrontZ);

	up.x = std::stof(UpX);
	up.y = std::stof(UpY);
	up.z = std::stof(UpZ);

	//Assign data to Frustum
	frustum.nearPlaneDistance = near_plane;
	frustum.farPlaneDistance = far_plane;
	frustum.verticalFov = fov;

	frustum.pos = pos;
	frustum.front = front;
	frustum.up = up;

	this->SetAspectRatio(aspect_ratio);
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

/* Specifying(-1, -1) returns the bottom - left corner of the near plane.
	The point(1, 1) corresponds to the top - right corner of the near plane. */
Ray ComponentCamera::GetEyeProjectedRay(float x, float y) const
{
	return frustum.UnProject(x, y);
}

Ray ComponentCamera::GetNearProjectedRay(float x, float y) const
{
	return frustum.UnProjectFromNearPlane(x, y);
}

LineSegment ComponentCamera::GetNearProjSegment(float x, float y) const
{
	return frustum.UnProjectLineSegment(x, y);
}

bool ComponentCamera::Intersects(const AABB& refBox) const
{
	return Intersects(frustum, refBox);
}

bool ComponentCamera::Intersects(const Frustum& refFrustum, const AABB& refBox)	// We use this instead of MathGeoLib's method because it's quicker
{
	float3 corners[8];
	refBox.GetCornerPoints(corners);

	Plane p[6];
	refFrustum.GetPlanes(p);

	uint totalInside = 0;

	for (int i = 0; i < 6; ++i) {	// For each frustum plane
		int insideCount = 8;

		for (int j = 0; j < 8; ++j)	// For each AABB corner
			if (p[i].IsOnPositiveSide(corners[j]))	// Frustum plane normals point outside the Frustum volume, therefore "out = positive side"
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