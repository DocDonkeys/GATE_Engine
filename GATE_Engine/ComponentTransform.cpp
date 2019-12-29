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
	else if (needsUpdateLocal) {
		UpdateLocalMat();
		needsUpdateLocal = false;
	}

	/*if (needsUpdateLocal) {
		UpdateLocalMat();
		needsUpdateLocal = false;
	}*/
}

void ComponentTransform::UpdateGlobalMat()
{
	ComponentTransform* parentTrs = (ComponentTransform*)my_go->parent->GetComponent(COMPONENT_TYPE::TRANSFORM);

	float4x4 newGlobal = parentTrs->globalTrs * localTrs;
	globalTrs = newGlobal;
	//globalTrs.SetTranslatePart(newGlobal.TranslatePart());
	//globalTrs.SetRotatePart(globalQuat);
	//globalTrs.Scale(newGlobal.GetScale() - globalTrs.GetScale());

	my_go->UpdateBoundingBox();

	for (int i = 0; i < my_go->children.size(); i++) {
		ComponentTransform* childTrs = (ComponentTransform*)my_go->children[i]->GetComponent(COMPONENT_TYPE::TRANSFORM);
		childTrs->UpdateGlobalMat();
	}
}

void ComponentTransform::UpdateLocalMat()
{
	ComponentTransform* parentTrs = (ComponentTransform*)my_go->parent->GetComponent(COMPONENT_TYPE::TRANSFORM);

	float4x4 newLocal = parentTrs->globalTrs.Inverted() * globalTrs;
	localTrs = newLocal;
	//localTrs.SetTranslatePart(newLocal.TranslatePart());
	//localTrs.SetRotatePart(localQuat);
	//localTrs.Scale(newLocal.GetScale() - localTrs.GetScale());

	my_go->UpdateBoundingBox();

	position = localTrs.TranslatePart();
	rotation = localTrs.ToEulerXYZ();
	scale = localTrs.GetScale();

	for (int i = 0; i < my_go->children.size(); i++) {
		ComponentTransform* childTrs = (ComponentTransform*)my_go->children[i]->GetComponent(COMPONENT_TYPE::TRANSFORM);
		childTrs->UpdateGlobalMat();
	}
}

float3 ComponentTransform::Translate(float3 movement, bool local)
{
	if (local) {
		float3 origPos = position;
		localTrs = localTrs * float4x4::Translate(movement);
		position = localTrs.TranslatePart();
		needsUpdateGlobal = true;

		return position - origPos;
	}
	else {
		float3 origPos = globalTrs.TranslatePart();
		globalTrs = globalTrs * float4x4::Translate(movement);
		needsUpdateLocal = true;

		return globalTrs.TranslatePart() - origPos;
	}
}

float3 ComponentTransform::SetPosition(float3 targetPos, bool local)
{
	if (local) {
		float3 origPos = position;
		localTrs.SetTranslatePart(targetPos);
		position = localTrs.TranslatePart();
		needsUpdateGlobal = true;

		return position - origPos;
	}
	else {
		float3 origPos = globalTrs.TranslatePart();;
		globalTrs.SetTranslatePart(targetPos);
		needsUpdateLocal = true;

		return globalTrs.TranslatePart() - origPos;
	}
}

float3 ComponentTransform::Rotate(float3 rot, bool local)
{
	if (local) {
		float3 origRot = rotation;
		localTrs = localTrs * Quat::RotateAxisAngle(localTrs.WorldX(), rot.x) * Quat::RotateAxisAngle(localTrs.WorldY(), rot.y) * Quat::RotateAxisAngle(localTrs.WorldZ(), rot.z);
		rotation = localTrs.ToEulerXYZ();
		needsUpdateGlobal = true;

		return rotation - origRot;
	}
	else {
		float3 origRot = globalTrs.ToEulerXYZ();
		globalTrs = globalTrs * Quat::RotateX(rot.x) * Quat::RotateY(rot.y) * Quat::RotateZ(rot.z);
		needsUpdateLocal = true;

		return globalTrs.ToEulerXYZ() - origRot;
	}
}

float3 ComponentTransform::Rotate(Quat rot, bool local)
{
	if (local) {
		float3 origRot = rotation;
		localTrs = localTrs * rot;
		rotation = localTrs.ToEulerXYZ();
		needsUpdateGlobal = true;

		return rotation - origRot;
	}
	else {
		float3 origRot = globalTrs.ToEulerXYZ();
		globalTrs = globalTrs * rot;
		needsUpdateLocal = true;

		return globalTrs.ToEulerXYZ() - origRot;
	}
}

float3 ComponentTransform::SetRotation(float3 targetRot, bool local)
{
	if (local) {
		float3 origRot = rotation;
		localTrs.SetRotatePart(Quat::FromEulerXYZ(targetRot.x, targetRot.y, targetRot.z));
		rotation = targetRot;
		needsUpdateGlobal = true;

		return localTrs.ToEulerXYZ() - origRot;
	}
	else {
		float3 origRot = globalTrs.ToEulerXYZ();
		globalTrs.SetRotatePart(Quat::FromEulerXYZ(targetRot.x, targetRot.y, targetRot.z));
		needsUpdateLocal = true;

		return globalTrs.ToEulerXYZ() - origRot;
	}
}

float3 ComponentTransform::SetRotation(Quat targetRot, bool local)
{
	if (local) {
		float3 origRot = rotation;
		localTrs.SetRotatePart(targetRot);
		rotation = targetRot.ToEulerXYZ();
		needsUpdateGlobal = true;

		return localTrs.ToEulerXYZ() - origRot;
	}
	else {
		float3 origRot = globalTrs.ToEulerXYZ();
		globalTrs.SetRotatePart(targetRot);
		needsUpdateLocal = true;

		return globalTrs.ToEulerXYZ() - origRot;
	}
}

float3 ComponentTransform::Scale(float3 scale, bool local)
{
	if (local) {
		float3 origScale = this->scale;
		localTrs.Scale(scale);
		this->scale += scale;

		needsUpdateGlobal = true;
		return this->scale - origScale;
	}
	else {
		float3 origScale = globalTrs.GetScale();
		globalTrs.Scale(scale);

		needsUpdateLocal = true;
		return globalTrs.GetScale() - origScale;
	}
}

float3 ComponentTransform::SetScale(float3 targetScale, bool local)
{
	if (local) {
		float3 origScale = this->scale;
		localTrs.Scale(float3(-this->scale.x, -this->scale.y, -this->scale.z));	// Scale to 0
		localTrs.Scale(targetScale);
		this->scale = targetScale;

		needsUpdateGlobal = true;
		return this->scale - origScale;
	}
	else {
		float3 origScale = globalTrs.GetScale();
		globalTrs.Scale(float3(-this->scale.x, -this->scale.y, -this->scale.z));	// Scale to 0
		globalTrs.Scale(targetScale);

		needsUpdateLocal = true;
		return globalTrs.GetScale() - origScale;
	}
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
	else if (scale.x != newScale.x
		|| scale.y != newScale.y
		|| scale.z != newScale.z) {
		changed = true;
	}
	else if (rotation.x != newRot.x
		|| rotation.y != newRot.y
		|| rotation.z != newRot.z) {
		changed = true;

		if (abs(rotation.x - newRot.x) < 0.000001 && abs(rotation.y - newRot.y) < 0.000001 && abs(rotation.z - newRot.z) < 0.000001)
			changed = false;	//CHANGE/FIX: Mousepicking rotates the clicked obj by very small decimals (wtf), this avoids a transformation to happen because of it
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

bool ComponentTransform::SetGlobalMat(float3& newPos, float3& newRot, float3& newScale)
{
	bool changed = false;
	bool mousePickFix = true;	// Removed when mouse picking bug is fixed

	float3 oldPos = globalTrs.TranslatePart();
	float3 oldRot = globalTrs.ToEulerXYZ();
	float3 oldScale = globalTrs.GetScale();

	if (oldPos.x != newPos.x
		|| oldPos.y != newPos.y
		|| oldPos.z != newPos.z) {
		changed = true;
	}
	else if (oldScale.x != newScale.x
		|| oldScale.y != newScale.y
		|| oldScale.z != newScale.z) {
		changed = true;
	}
	else if (oldRot.x != newRot.x
		|| oldRot.y != newRot.y
		|| oldRot.z != newRot.z) {
		changed = true;

		if (abs(oldRot.x - newRot.x) < 0.000001 && abs(oldRot.y - newRot.y) < 0.000001 && abs(oldRot.z - newRot.z) < 0.000001)
			changed = false;	//CHANGE/FIX: Mousepicking rotates the clicked obj by very small decimals (wtf), this avoids a transformation to happen because of it
	}

	if (changed) {
		globalTrs = float4x4::FromTRS(newPos, Quat::FromEulerXYZ(newRot.x, newRot.y, newRot.z), newScale);
		needsUpdateLocal = true;
	}

	return changed;
}

bool ComponentTransform::SetGlobalMat(float4x4& newMat)
{
	bool changed = false;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (globalTrs[i][j] != newMat[i][j]) {
				changed = true;
				break;
			}

	if (changed) {
		globalTrs = newMat;
		needsUpdateLocal = true;
	}

	return changed;
}

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
	rotation = localTrs.ToEulerXYZ();
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