#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"
#include "Mesh.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	~ComponentMesh();

public:
	void Enable() override;
	void Update() override;
	void Disable() override;

	void Draw();

public:
	Mesh* mesh = nullptr;
	std::string filename;
};

#endif // !__COMPONENTMESH_H__