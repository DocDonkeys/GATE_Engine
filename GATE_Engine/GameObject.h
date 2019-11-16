#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <vector>

#include "libs/MathGeoLib/include/Geometry/AABB.h"
#include "libs/MathGeoLib/include/Geometry/OBB.h"

#define OBJ_MAX_CHARS 50	//IMPROVE: This is far from perfect

class Component;

enum class COMPONENT_TYPE
{
	TRANSFORM = 0,
	MESH,
	MATERIAL,
	CAMERA
};
class GameObject
{
public:
	GameObject();
	GameObject(const char* name);
	GameObject(const char* name, float4x4 local_mat);
	~GameObject();

public: //Methods
	void PreUpdate();
	void Update();
	void PostUpdate();

	void Draw();	// This method is called by the renderer
	void DrawAABB();

	void UpdateBoundingBox(float4x4 mat);

	Component* CreateComponent(COMPONENT_TYPE type);
	Component* GetComponent(COMPONENT_TYPE type);

public: //Vars
	bool active = true;
	bool staticObj = true;
	std::string name;
	std::vector<Component*> components;

	//Gameobject hierarchy tree related Vars
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	AABB aabb;
	OBB obb;
	float3 size;
};

//Namespace to carry out operations related to Game Objects
namespace GOFunctions {
	void ReParentGameObject(GameObject* child, GameObject* parent);
}
#endif // !__GAMEOBJECT_H__

