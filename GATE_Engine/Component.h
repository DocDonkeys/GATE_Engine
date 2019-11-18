#ifndef COMPONENT_H
#define COMPONENT_H

#include "GameObject.h"

class GameObject;

class Component
{
public:
	Component() {};
	virtual ~Component() {};

public: //Methods
	virtual void Enable() = 0;
	virtual void Disable() = 0;

	virtual void PreUpdate() {};
	virtual void Update() {};
	virtual void PostUpdate() {};
	virtual void Draw() {};

public: //Members
	COMPONENT_TYPE type;

	bool active = false;
	GameObject* my_go = nullptr;

};
#endif // !__COMPONENT_H__

