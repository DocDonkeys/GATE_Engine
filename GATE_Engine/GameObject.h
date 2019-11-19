#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__
#include "Globals.h"
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
	~GameObject();

public: //Methods
	void PreUpdate();
	void Update();
	void PostUpdate();

	void Draw() const;	// This method is called by the renderer
	void DrawAABB() const;
	void UpdateBoundingBox();

	void UpdateStaticStatus(bool newVal);

	Component* CreateComponent(COMPONENT_TYPE type);
	Component* GetComponent(COMPONENT_TYPE type) const;

public: //Vars
	uint32_t UID = 0;
	uint32_t parent_UID = 0;
	bool active = true;
	bool staticObj = false;
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

	//Returns all gameobjects that are directly or indirectly children of the gameobject, for instance: 
	// if a children gameobject also has children, it will include its children too, including all nodes of the tree that depend on the gameobject passed
	void FillArrayWithChildren(std::vector<const GameObject*> &go_array, const GameObject* go, bool add_parent = false, uint count = 0);
}
#endif // !__GAMEOBJECT_H__

