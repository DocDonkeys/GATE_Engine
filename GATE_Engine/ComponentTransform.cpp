#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
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

bool ComponentTransform::SetLocalMat(float3& newPos, float3& newRot, float3& newScale)
{
	bool changed = false;

	if (position.x != newPos.x
		|| position.y != newPos.y
		|| position.z != newPos.z) {
		changed = true;
	}
	else if (rotation.x != newRot.x
		|| rotation.y != newRot.y
		|| rotation.z != newRot.z) {
		changed = true;
	}
	else if (scale.x != newScale.x
		|| scale.y != newScale.y
		|| scale.z != newScale.z) {
		changed = true;
	}

	if (changed) {
		if (abs(rotation.x - newRot.x) < 0.000001 || abs(rotation.y - newRot.y) < 0.000001 || abs(rotation.z - newRot.z) < 0.000001)
			changed = false;	//CHANGE/FIX: Mousepicking rotates the clicked obj by very small decimals (wtf), this avoids the object from becoming dynamic (if it was static)

		localTrs = float4x4::FromTRS(newPos, float3x3::FromEulerXYZ(newRot.x, newRot.y, newRot.z), newScale);
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
			global[k] = localTrs[i][j];
			k++;
		}
}