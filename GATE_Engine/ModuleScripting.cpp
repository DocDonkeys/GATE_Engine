#include "ModuleScripting.h"

extern "C"
{
#include "libs/lua-535/include/lua.h"
#include "libs/lua-535/include/lauxlib.h"
#include "libs/lua-535/include/lualib.h"
}
//This include MUST go after Lua includes
#include "libs/LuaBridge-241/include/LuaBridge.h"


#pragma comment(lib, "libs/lua-535/liblua53.a")

ModuleScripting::ModuleScripting(Application * app, const char * name, bool start_enabled) : Module(app, name, start_enabled)
{
}

ModuleScripting::~ModuleScripting()
{
}

bool ModuleScripting::Init()
{
	//Create the Virtual Machine
	L = luaL_newstate();


	return true;
}

bool ModuleScripting::Start()
{
	std::string test = "r = 10 + 14";

	int r = luaL_dostring(L, test.c_str());

	if (r == LUA_OK)
	{
		lua_getglobal(L,"r");
		int a = lua_tointeger(L,-1);
		LOG("Lua operation %s = %d", test.data(), a);
	}

	

	return true;
}

bool ModuleScripting::CleanUp()
{
	return true;
}

update_status ModuleScripting::Update(float dt)
{
	

	return UPDATE_CONTINUE;
}
