#include "ModuleScripting.h"
#include "Application.h"
#include "ModuleFileSystem.h"

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
	
	std::string script_path = App->file_system->GetPathToGameFolder(true) + "/Assets/Scripts/" + "lua_test.lua";
	bool compiled = luaL_dofile(L, script_path.c_str());

	if (compiled == LUA_OK)
	{
		if (start == true)
		{
			luabridge::LuaRef ScriptStart = luabridge::getGlobal(L, "Start");
			if (!ScriptStart.isNil())
				ScriptStart();
			else
				LOG("Could not execute Start!");
		}
		start = false;
		//Get the Update function from LUA file
		luabridge::LuaRef ScriptUpdate = luabridge::getGlobal(L,"Update");
		//Execute Update
		if(!ScriptUpdate.isNil())
			for (int i = 0; i < 35; ++i)
			{
				ScriptUpdate();

				luabridge::LuaRef number = luabridge::getGlobal(L, "Update_test");

				int num = 0;
				if (!number.isNil())
					num = number.cast<int>();

				LOG("Lua script Update was called. Update_test = %d", num);
			}
		

		
	}

	return UPDATE_CONTINUE;
}
