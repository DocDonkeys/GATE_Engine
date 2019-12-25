#include "ModuleScripting.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneIntro.h"
#include "Resource.h"
#include "ResourceScript.h"
#include "ComponentScript.h"
#include <iterator>


extern "C"
{
#include "libs/lua-535/include/lua.h"
#include "libs/lua-535/include/lauxlib.h"
#include "libs/lua-535/include/lualib.h"
}
//This include MUST go after Lua includes
#include "libs/LuaBridge-241/include/LuaBridge.h"
#include "ScriptData.h"

//LUA lib
#pragma comment(lib, "libs/lua-535/liblua53.a")

ModuleScripting::ModuleScripting(Application * app, const char * name, bool start_enabled) : Module(app, name, start_enabled)
{
}

ModuleScripting::~ModuleScripting()
{
}

void ModuleScripting::CompileScriptTableClass(ScriptInstance * script)
{

	luabridge::getGlobalNamespace(L)
		.beginNamespace("Debug")
		.beginClass <Scripting>("Scripting")
		.addConstructor<void(*) (void)>()
		.addFunction("LOG", &Scripting::LogFromLua)
		.endClass()
		.endNamespace();

	Scripting Scripting;

	if (L != nullptr)
	{
		//Compile the file and run it, we're gonna optimize this to just compile the function the script contains to library later.
		int compiled = luaL_dofile(L, script->my_resource->script_file.c_str());

		if (compiled == LUA_OK)
		{
			//Get the function to instantiate the lua table (used as a class as known in C++)
			std::string get_function = "GetTable" + script->my_resource->script_name;
			get_function = App->SubtractString(get_function,".",false,true,false);
			luabridge::LuaRef ScriptGetTable = luabridge::getGlobal(L, get_function.c_str());
			
			if (!ScriptGetTable.isNil())
			{
				luabridge::LuaRef table(ScriptGetTable());
				//table["Update"];
				//Assign the Table instance returned by the function to our script instance
				//luabridge::LuaRef table = luabridge::getGlobal(L, "c_table");
				script->my_table_class = table;

				int testing = 0;
			}
				
		}
		else
		{
			std::string error = lua_tostring(L, -1);
			LOG("%s", error.data());
		}
	}
}

void ModuleScripting::SendScriptToModule(ComponentScript * script_component, std::string full_file_path)
{
	//ScriptFile* s_file = AddScriptFile(script_component, full_file_path);

	ScriptInstance* s_instance = new ScriptInstance;
	s_instance->my_component = script_component;
	s_instance->my_resource = script_component->script;

	class_instances.push_back(s_instance);
	CompileScriptTableClass(s_instance); //Compile so we can give the instance its table/class reference
}

ScriptFile* ModuleScripting::AddScriptFile(ComponentScript* script_component, std::string full_file_path)
{
	ScriptFile* ret = nullptr;
	std::string filename;
	App->file_system->SplitFilePath(full_file_path.data(),nullptr,&filename,nullptr);
	for (std::vector<ScriptFile*>::iterator it = script_files.begin(); it != script_files.end(); ++it)
	{
		//If this file is already in our list of files
		Resource* res = (Resource*)script_component->script;
		if ((*it)->resource_uid == res->GetUID())
		{
			return (*it);
		}
	}

	ScriptFile* new_file = new ScriptFile;
	script_files.push_back(new_file);
	ret = new_file;
		
	return ret;
}

bool ModuleScripting::Init()
{
	//Create the Virtual Machine
	L = luaL_newstate();
	luaL_openlibs(L);

	return true;
}

bool ModuleScripting::Start()
{

	return true;
}

bool ModuleScripting::CleanUp()
{
	return true;
}

update_status ModuleScripting::Update(float dt)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Debug")
		.beginClass <Scripting>("Scripting")
		.addConstructor<void(*) (void)>()
		.addFunction("LOG", &Scripting::LogFromLua)
		.endClass()
		.endNamespace();

	Scripting Scripting;
	//Building a class / Namespace so Lua can have this object to Call EngineLOG by calling 
	if (App->scene_intro->playing == true)
	{
		for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
		{
			if ((*it)->my_component->active == true) //Check if the script instance is active or not
			{
				if (start == true)
				{
					//Call Start Method of LUA class
					(*it)->my_table_class["Start"] ();
				}
				else
				{
					//Call Update Method of LUA class
					(*it)->my_table_class["Update"] ();
				}
			}
		}
		start = false;
		/*luabridge::getGlobalNamespace(L)
			.beginNamespace("Debug")
			.beginClass <Scripting>("Scripting")
			.addConstructor<void(*) (void)>()
			.addFunction("LOG", &Scripting::LogFromLua)
			.endClass()
			.endNamespace();

		Scripting Scripting;*/

		//std::string script_path = App->file_system->GetPathToGameFolder(true) + "/Assets/Scripts/" + "lua_tabletest.lua";
		//bool compiled = luaL_dofile(L, script_path.c_str());

		//if (compiled == LUA_OK)
		//{
		//	if (start == true)
		//	{
		//		luabridge::LuaRef ScriptStart = luabridge::getGlobal(L, "Start");
		//		if (!ScriptStart.isNil())
		//			ScriptStart();
		//		else
		//			LOG("Could not execute Start!");
		//	}
		//	start = false;
		//	//Get the Update function from LUA file
		//	luabridge::LuaRef ScriptUpdate = luabridge::getGlobal(L, "Update");
		//	//Execute Update
		//	if (!ScriptUpdate.isNil())
		//		for (int i = 0; i < 35; ++i)
		//		{
		//			ScriptUpdate();

		//			luabridge::LuaRef number = luabridge::getGlobal(L, "Update_test");

		//			int num = 0;
		//			if (!number.isNil())
		//				num = number.cast<int>();

		//			LOG("Lua script Update was called. Update_test = %d", num);
		//		}



		//}
		//else
		//{
		//	std::string error = lua_tostring(L, -1);
		//	LOG("%s", error.data());
		//}
	}
	else
	{
		start = true;
	}

	return UPDATE_CONTINUE;
}


Scripting::Scripting()
{
}

Scripting::~Scripting()
{
}

//Function that Lua will be able to call as LOG
void Scripting::LogFromLua(const char * string)
{
	LOG(string);
}

void Scripting::TestFunc()
{
	LOG("This is a test function that Logs something (TestFunc)");
}

//void Scripting::GetRealTime(luabridge::LuaRef* target)
//{
//	(*target)["real_time"] = App->GetRealTime();
//}
//
//void Scripting::GetGameTime(luabridge::LuaRef* target)
//{
//	(*target)["game_time"] = App->GetGameTime();
//}