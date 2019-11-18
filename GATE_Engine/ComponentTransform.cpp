#include "Globals.h"
#include "Application.h"
#include "ComponentTransform.h"

#include "libs/MathGeoLib/include/Math/MathFunc.h"
#include "libs/SDL/include/SDL_assert.h"

ComponentTransform::ComponentTransform() : Component()
{
	type = COMPONENT_TYPE::TRANSFORM;
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Enable()
{
	active = true;
}

void ComponentTransform::Disable()
{
	active = false;
}

void ComponentTransform::PreUpdate()
{
	if (my_go->parent == nullptr) {
		SDL_assert(my_go == App->scene_intro->root);
	}

	if (needsMatUpdate) {
		UpdateGlobalMat();
		needsMatUpdate = false;
	}
}

void ComponentTransform::UpdateGlobalMat()
{
	ComponentTransform* parentTrs = (ComponentTransform*)my_go->parent->GetComponent(COMPONENT_TYPE::TRANSFORM);
	globalTrs = parentTrs->globalTrs * localTrs;
	my_go->UpdateBoundingBox(globalTrs);

	for (int i = 0; i < my_go->children.size(); i++) {
		ComponentTransform* childTrs = (ComponentTransform*)my_go->children[i]->GetComponent(COMPONENT_TYPE::TRANSFORM);
		childTrs->UpdateGlobalMat();
	}
}

//void ComponentTransform::UpdateQuatByEuler(float3& newEuler)
//{
//	float3 rotAng = DegToRad(newEuler - eulerRotation);
//	quatRotation = quatRotation * Quat::FromEulerXYZ(rotAng.x, rotAng.y, rotAng.z);
//}
//
//void ComponentTransform::UpdateEulerByQuat(Quat& q)
//{
//	eulerRotation = RadToDeg(q.ToEulerXYZ());
//}

bool ComponentTransform::DataToMat(float3& newPos, float3& newRot, float3& newScale)
{
	bool changed = false;

	float3 pos = localTrs.TranslatePart();
	float3 rot = localTrs.ToEulerXYZ();
	float3 scale = localTrs.GetScale();

	if (pos.x != newPos.x
		|| pos.y != newPos.y
		|| pos.z != newPos.z)
		changed = true;
	else if (rot.x != newRot.x
		|| rot.y != newRot.y
		|| rot.z != newRot.z)
		changed = true;
	else if (scale.x != newScale.x
		|| scale.y != newScale.y
		|| scale.z != newScale.z)
		changed = true;

	if (changed) {
		localTrs = float4x4::FromTRS(newPos, Quat::FromEulerXYZ(newRot.x, newRot.y, newRot.z), newScale);
		needsMatUpdate = true;
	}

	return changed;
}

void ComponentTransform::MatToData(float3& givenPos, float3x3& givenRot, float3& givenScale)
{
	localTrs.Decompose(givenPos, givenRot, givenScale);
}

void ComponentTransform::MatToData(float3& givenPos, Quat& givenQuat, float3& givenScale)
{
	localTrs.Decompose(givenPos, givenQuat, givenScale);
}