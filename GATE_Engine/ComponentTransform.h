#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "libs/MathGeoLib/include/Math/float3.h"
#include "libs/MathGeoLib/include/Math/Quat.h"
#include "libs/MathGeoLib/include/Math/float4x4.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform();

public:
	void Enable() override;
	void PreUpdate() override;
	void Disable() override;

public:
	float3 position;
	math::Quat rotation;
	float3 scale;

	math::float4x4 localMat;
	math::float4x4 globalMat;
};

#endif // !__COMPONENTTRANSFORM_H__

