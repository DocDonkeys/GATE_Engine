#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	~ComponentMesh();

public:
	void Enable() override;
	void Update() override;
	void Disable() override;
};

#endif // !__COMPONENTMESH_H__