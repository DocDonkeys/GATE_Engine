#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <vector>

#define OBJ_MAX_CHARS 50	//IMPROVE: This is far from perfect

class Component;

enum class COMPONENT_TYPE
{
	TRANSFORM = 0,
	MESH,
	MATERIAL
};
class GameObject
{
public:
	GameObject();
	GameObject(const char* name);
	~GameObject();

public: //Methods
	void Update();
	Component* CreateComponent(COMPONENT_TYPE type);
	Component* GetComponent(COMPONENT_TYPE type);

	//Change the parent of the GamObject with its new parent
	void ReParent(GameObject* new_parent);

public: //Vars
	bool active = true;
	bool staticObj = true;
	std::string name;
	std::vector<Component*> components;

	//Gameobject hierarchy tree related Vars
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};
#endif // !__GAMEOBJECT_H__

