#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"
#include "Mesh.h"
#include "JsonLoader.h"
#include "ImporterMesh.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	~ComponentMesh();

public:
	void Enable() override;
	void Disable() override;

	void Draw();

	void Save(json &file);
	void Load(json &file);

public:
	Mesh* mesh = nullptr;

	bool debug_vertex_normals = false;
	bool debug_face_normals = false;
	
	ImporterMesh imp_exp;
	IEMeshData ie_data;
};

#endif // !__COMPONENTMESH_H__
