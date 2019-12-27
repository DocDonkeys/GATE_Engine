#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ComponentTransform.h"

#include "libs/MathGeoLib/include/Math/MathFunc.h"
#include "libs/MathGeoLib/include/Math/TransformOps.h"
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

void ComponentTransform::PreUpdate(float realDT)
{
	if (my_go->parent == nullptr) {
		SDL_assert(my_go == App->scene_intro->root);
		needsUpdateGlobal = false;
	}

	if (needsUpdateGlobal) {
		UpdateGlobalMat();
		needsUpdateGlobal = false;
	}

	/*if (needsUpdateLocal) {
		UpdateLocalMat();
		needsUpdateLocal = false;
	}*/
}

void ComponentTransform::UpdateGlobalMat()
{
	ComponentTransform* parentTrs = (ComponentTransform*)my_go->parent->GetComponent(COMPONENT_TYPE::TRANSFORM);
	globalTrs = parentTrs->globalTrs * localTrs;
	my_go->UpdateBoundingBox();

	for (int i = 0; i < my_go->children.size(); i++) {
		ComponentTransform* childTrs = (ComponentTransform*)my_go->children[i]->GetComponent(COMPONENT_TYPE::TRANSFORM);
		childTrs->UpdateGlobalMat();
	}
}

//void ComponentTransform::UpdateLocalMat()
//{
//	ComponentTransform* parentTrs = (ComponentTransform*)my_go->parent->GetComponent(COMPONENT_TYPE::TRANSFORM);
//	//globalTrs = parentTrs->globalTrs * localTrs;
//	my_go->UpdateBoundingBox(globalTrs);
//
//	for (int i = 0; i < my_go->children.size(); i++) {
//		ComponentTransform* childTrs = (ComponentTransform*)my_go->children[i]->GetComponent(COMPONENT_TYPE::TRANSFORM);
//		childTrs->UpdateLocalMat();
//	}
//}

float3 ComponentTransform::Translate(float3 movement)
{
	float3 origPos = position;
	localTrs = localTrs * float4x4::Translate(movement);
	position = localTrs.TranslatePart();
	needsUpdateGlobal = true;

	return position - origPos;
}

float3 ComponentTransform::SetTranslation(float3 targetPos)
{
	float3 origPos = position;
	localTrs = localTrs.FromTRS(targetPos, localTrs.RotatePart(), scale);
	position = localTrs.TranslatePart();
	needsUpdateGlobal = true;

	return position - origPos;
}

float3 ComponentTransform::Rotate(float3 rot)
{
	float3 origRot = rotation;
	localTrs = localTrs * localTrs.RotateX(rot.x) * localTrs.RotateY(rot.y) * localTrs.RotateZ(rot.z);
	rotation = localTrs.RotatePart().ToEulerXYZ();
	needsUpdateGlobal = true;

	return rotation - origRot;
}

float3 ComponentTransform::SetRotation(float3 targetRot)
{
	float3 origRot = rotation;
	localTrs = localTrs.FromTRS(position, Quat::FromEulerXYZ(targetRot.x, targetRot.y, targetRot.z), scale);
	rotation = localTrs.RotatePart().ToEulerXYZ();
	needsUpdateGlobal = true;

	return localTrs.RotatePart().ToEulerXYZ() - origRot;
}

float3 ComponentTransform::Scale(float3 scale)
{
	float3 origScale = this->scale;
	localTrs.Scale(scale);
	this->scale += scale;

	needsUpdateGlobal = true;
	return this->scale - origScale;
}

float3 ComponentTransform::SetScale(float3 targetScale)
{
	float3 origScale = this->scale;
	localTrs.Scale(float3(-this->scale.x, -this->scale.y, -this->scale.z));	// Scale to 0
	localTrs.Scale(targetScale);
	this->scale = targetScale;

	needsUpdateGlobal = true;
	return this->scale - origScale;
}

bool ComponentTransform::SetLocalMat(float3& newPos, float3& newRot, float3& newScale)
{
	bool changed = false;
	bool mousePickFix = true;	// Removed when mouse picking bug is fixed

	if (position.x != newPos.x
		|| position.y != newPos.y
		|| position.z != newPos.z) {
		changed = true;
	}
	else if (rotation.x != newRot.x
		|| rotation.y != newRot.y
		|| rotation.z != newRot.z) {
		changed = true;

		if (abs(rotation.x - newRot.x) < 0.000001 && abs(rotation.y - newRot.y) < 0.000001 && abs(rotation.z - newRot.z) < 0.000001)
			changed = false;	//CHANGE/FIX: Mousepicking rotates the clicked obj by very small decimals (wtf), this avoids a transformation to happen because of it
	}
	else if (scale.x != newScale.x
		|| scale.y != newScale.y
		|| scale.z != newScale.z) {
		changed = true;
	}

	if (changed) {
		localTrs = float4x4::FromTRS(newPos, Quat::FromEulerXYZ(newRot.x, newRot.y, newRot.z), newScale);
		position = newPos;
		rotation = newRot;
		scale = newScale;

		needsUpdateGlobal = true;
	}

	return changed;
}

bool ComponentTransform::SetLocalMat(float4x4& newMat)
{
	bool changed = false;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (localTrs[i][j] != newMat[i][j]) {
				changed = true;
				break;
			}

	if (changed) {
		localTrs = newMat;
		position = newMat.TranslatePart();
		rotation = newMat.ToEulerXYZ();
		scale = newMat.GetScale();

		needsUpdateGlobal = true;
	}

	return changed;
}

//bool ComponentTransform::SetGlobalMat(float3& newPos, float3& newRot, float3& newScale)
//{
//	bool changed = false;
//
//	if (position.x != newPos.x
//		|| position.y != newPos.y
//		|| position.z != newPos.z) {
//		changed = true;
//	}
//	else if (rotation.x != newRot.x
//		|| rotation.y != newRot.y
//		|| rotation.z != newRot.z) {
//		changed = true;
//	}
//	else if (scale.x != newScale.x
//		|| scale.y != newScale.y
//		|| scale.z != newScale.z) {
//		changed = true;
//	}
//
//	if (changed) {
//		globalTrs = float4x4::FromTRS(newPos, float3x3::FromEulerXYZ(newRot.x, newRot.y, newRot.z), newScale);
//		needsUpdateLocal = true;
//	}
//
//	return changed;
//}
//
//bool ComponentTransform::SetGlobalMat(float4x4& newMat)
//{
//	bool changed = false;
//
//	for (int i = 0; i < 4; i++)
//		for (int j = 0; j < 4; j++)
//			if (localTrs[i][j] != newMat[i][j]) {
//				changed = true;
//				break;
//			}
//
//	if (changed) {
//		globalTrs = newMat;
//		needsUpdateLocal = true;
//	}
//
//	return changed;
//}

void ComponentTransform::GetLocalMat(float3& pos, float3& rot, float3& scale)
{
	pos = position;
	rot = rotation;
	scale = this->scale;
}

void ComponentTransform::GetLocalMat(float3& pos, float3x3& rot, float3& scale)
{
	localTrs.Decompose(pos, rot, scale);
}

float4x4 ComponentTransform::GetLocalMat() const
{
	return localTrs;
}

float4x4 ComponentTransform::GetGlobalMat() const
{
	return globalTrs;
}

void ComponentTransform::Import(float* local, float* global)
{
	int k = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			localTrs[i][j] = local[k];
			k++;
		}

	position = localTrs.TranslatePart();
	rotation = localTrs.RotatePart().ToEulerXYZ();
	scale = localTrs.GetScale();

	k = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			globalTrs[i][j] = global[k];
			k++;
		}

	needsUpdateGlobal = true;
}

void ComponentTransform::Export(float* local, float* global)
{
	int k = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			local[k] = localTrs[i][j];
			k++;
		}

	k = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			global[k] = globalTrs[i][j];
			k++;
		}
}