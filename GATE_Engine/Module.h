#ifndef MODULE_H
#define MODULE_H

#include "Globals.h"
#include <string>

class Application;

class Module
{
private :
	bool enabled;

public:
	Application* App;

	Module(Application* parent, const char* name = "null", bool start_enabled = true) : App(parent), name(name), active(start_enabled)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

public:
	std::string name;
	bool active;
};

#endif //MODULE_H