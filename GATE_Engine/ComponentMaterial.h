#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	~ComponentMaterial();

public:
	void Enable() override;
	void Update() override;
	void Disable() override;
};

#endif // !__COMPONENTMATERIAL_H__
