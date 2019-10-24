#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <vector>

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
	~GameObject();

public: //Methods
	void Update();
	Component* CreateComponent(COMPONENT_TYPE type);
	Component* GetComponent(COMPONENT_TYPE type);

public: //Vars
	bool active = true;
	std::string name;
	std::vector<Component*> components;

	//Gameobject hierarchy tree related Vars
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};
#endif // !__GAMEOBJECT_H__

