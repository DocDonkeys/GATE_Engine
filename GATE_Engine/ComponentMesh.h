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

	bool debug_vertex_normals = false;
	bool debug_face_normals = false;
};

#endif // !__COMPONENTMESH_H__
