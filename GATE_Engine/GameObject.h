#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <vector>

class GameObject
{
public:
	GameObject();
	~GameObject();

public: //Methods
	void Update();
	//Component* CreateComponent();

public: //Vars
	std::string name;
	//std::vector<Component*> 

	//Gameobject hierarchy tree related Vars
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};
#endif // !__GAMEOBJECT_H__

