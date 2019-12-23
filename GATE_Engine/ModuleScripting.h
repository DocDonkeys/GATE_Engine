#ifndef __MODULESCRIPTING_H__
#define __MODULESCRIPTING_H__

#include "Module.h"

class lua_State;
class ModuleScripting : public Module
{
public:
	ModuleScripting(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleScripting();

public:

	bool Init();
	bool Start();
	bool CleanUp();
	update_status Update(float dt);

private:
	// L is our Lua Virtual Machine, it's called L because its the common name it receives, so all programers can understand what this var is
	lua_State *L = nullptr;
	bool start = true;
};


class Scripting
{
public:
	Scripting();
	~Scripting();

public:

	void LogFromLua(const char* string);
	void TestFunc();
};


#endif // !__MODULESCRIPTING_H__

