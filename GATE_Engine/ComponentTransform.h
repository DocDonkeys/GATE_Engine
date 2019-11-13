#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform();

public:
	void Enable() override;
	void Disable() override;
	void PreUpdate() override;

	void UpdateGlobalMat();

	bool UpdateValues(float3& pos, float3& rot, float3& scale);
	void UpdateQuatByEuler(float3& newEuler);
	void DataToMat();
	void MatToData();

public:
	// Data
	float3 position = float3::zero;
	Quat quatRotation = Quat::identity;
	float3 eulerRotation = float3::zero;
	float3 scale = float3::one;

	// Matrices
	float4x4 localTrs = float4x4::identity;
	float4x4 globalTrs = float4x4::identity;

	bool needsMatUpdate = false;
};

#endif // !__COMPONENTTRANSFORM_H__

