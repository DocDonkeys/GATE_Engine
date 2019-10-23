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
	void Update() override;
	void Disable() override;
};

#endif // !__COMPONENTTRANSFORM_H__

