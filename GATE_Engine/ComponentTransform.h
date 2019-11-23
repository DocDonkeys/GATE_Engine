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
	void PreUpdate(float realDT) override;

	//void UpdateLocalMat();
	void UpdateGlobalMat();

	//void UpdateQuatByEuler(float3& newEuler);
	//void UpdateEulerByQuat(Quat& q);
	bool SetLocalMat(float3& pos, float3& rot, float3& scale);
	bool SetLocalMat(float4x4& mat);

	//bool SetGlobalMat(float3& pos, float3& rot, float3& scale);	//IMPROVE: We might need to arbitrarily set the global position, but what should we do with the children? Move them or update their data?
	//bool SetGlobalMat(float4x4& mat);

	void GetLocalMat(float3& pos, float3& rot, float3& scale);
	void GetLocalMat(float3& pos, float3x3& rot, float3& scale);
	float4x4 GetLocalMat();
	float4x4 GetGlobalMat();

	void Import(float* local, float* global);
	void Export(float* local, float* global);

public:
	// These are needed because directly using matrices results in a big mess
	float3 position = float3::zero;
	float3 rotation = float3::zero;
	float3 scale = float3::one;

	//bool needsUpdateLocal = false;
	bool needsUpdateGlobal = false;

public:	//CHANGE/FIX: Should be private, only public currently because of Didac's import/export system
	float4x4 localTrs = float4x4::identity;
	float4x4 globalTrs = float4x4::identity;
};

#endif // !__COMPONENTTRANSFORM_H__

