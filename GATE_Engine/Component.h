#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "GameObject.h"

class Component
{
public:
	Component();
	virtual ~Component() = 0;

public: //Methods
	virtual void Enable() = 0;
	virtual void Update() = 0;
	virtual void Disable() = 0;

public: //Members
	COMPONENT_TYPE type;
	bool active = false;
	GameObject* my_go = nullptr;

};
#endif // !__COMPONENT_H__

