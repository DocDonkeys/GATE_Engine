#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "libs/MathGeoLib/include/Math/float3.h"
#include "libs/MathGeoLib/include/Math/Quat.h"
#include "libs/MathGeoLib/include/Math/float3x3.h"
#include "libs/MathGeoLib/include/Math/float4x4.h"

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

	//void UpdateQuatByEuler(float3& newEuler);
	//void UpdateEulerByQuat(Quat& q);
	bool DataToMat(float3& pos, float3& rot, float3& scale);
	void MatToData(float3& pos, float3x3& rot, float3& scale);
	void MatToData(float3& pos, Quat& rot, float3& scale);

public:
	float4x4 localTrs = float4x4::identity;
	float4x4 globalTrs = float4x4::identity;

	bool needsMatUpdate = false;
};

#endif // !__COMPONENTTRANSFORM_H__

