#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "libs/MathGeoLib/include/Math/float3.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform();

public:
	void Enable() override;
	void Update() override;
	void Disable() override;

public:
	float3 position;
	float3 rotation;
	float3 scale;
};

#endif // !__COMPONENTTRANSFORM_H__

