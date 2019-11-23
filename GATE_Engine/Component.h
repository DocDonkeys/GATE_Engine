#ifndef COMPONENT_H
#define COMPONENT_H

#include "Globals.h"
#include "GameObject.h"
#include "JsonLoader.h"

class GameObject;

class Component
{
public:
	Component() {};
	virtual ~Component() {};

public: //Methods
	virtual void Enable() = 0;
	virtual void Disable() = 0;

	virtual void PreUpdate(float realDT) {};
	virtual void Update(float realDT) {};
	virtual void GameUpdate(float gameDT) {};
	virtual void PostUpdate(float realDT) {};
	virtual void Draw() {};

	virtual void Save(json& file);
	virtual void Load(json& file);

public: //Members
	COMPONENT_TYPE type;

	bool active = false;
	GameObject* my_go = nullptr;
	uint32 UID = 0;
};
#endif // !__COMPONENT_H__

