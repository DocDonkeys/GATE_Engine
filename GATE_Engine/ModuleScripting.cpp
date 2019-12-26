#include "ModuleScripting.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "Resource.h"
#include "ResourceScript.h"
#include "ComponentScript.h"
#include "ComponentTransform.h"
#include "ModuleResources.h"
#include "MemLeaks.h"
#include <iterator>

#include "libs/MathGeoLib/include/Math/float4x4.h"

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

void ModuleScripting::DoHotReloading()
{
	if (App->scene_intro->playing == false)
	{
		//We do the necessary Hot Reloading
		//for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
		//{
		//	//Remove the references to the data inside the virtual machine
		//	(*it)->my_table_class = 0;
		//}
		////Close the virtual machine & Destroy it
		//lua_close(L);

		////Create the new Virtual Machine
		//L = luaL_newstate();
		//luaL_openlibs(L);

		//Acquire All scripts to be compiled       (we will compile even scripts which are currently not attached to any gameobject)
		//to check if it still compiles after the change done in a given script which is unknown to us
		std::string extension = "lua";
		std::vector<std::string> files;
		App->resources->GetAllFilesWithExtension(extension,files,App->resources->assets_dir);

		hot_reloading_waiting = false;
	}
	else
	{
		hot_reloading_waiting = true;
	}
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
		int compiled = luaL_dofile(L, script->my_resource->absolute_path.c_str());

		if (compiled == LUA_OK)
		{
			//Get the function to instantiate the lua table (used as a class as known in C++)
			std::string get_function = "GetTable" + script->my_resource->script_name;
			get_function = App->SubtractString(get_function,".",false,true,false);
			luabridge::LuaRef ScriptGetTable = luabridge::getGlobal(L, get_function.c_str());
			
			if (!ScriptGetTable.isNil())
			{
				luabridge::LuaRef table(ScriptGetTable());

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

void ModuleScripting::ManageOrphanScript(std::string relative_path)
{
	//Create a new meta
	ie_scripts.ie_data.meta_path = relative_path;
	ie_scripts.CreateMeta(relative_path.data(), &ie_scripts.ie_data);
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
					int num = (*it)->my_table_class["position_x"];

					int testwork = 0;
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

		//If a script was changed during runtime, hot reload
		if (hot_reloading_waiting)
			DoHotReloading();
	}

	return UPDATE_CONTINUE;
}


Scripting::Scripting()
{
}

Scripting::~Scripting()
{
}

// ENGINE TRANSLATOR
// General

//Function that Lua will be able to call as LOG
void Scripting::LogFromLua(const char * string)
{
	LOG(string);
}

void Scripting::TestFunc()
{
	LOG("This is a test function that Logs something (TestFunc)");
}

uint Scripting::GetRealTime()
{
	return App->GetRealTime();
}

uint Scripting::GetGameTime()
{
	return App->GetGameTime();
}

// Input
int Scripting::GetKeyState(int keyCode) const
{
	return App->input->GetKey(keyCode);
}

bool Scripting::KeyDown(int keyCode) const
{
	return App->input->GetKey(keyCode) == KEY_DOWN;
}

bool Scripting::KeyUp(int keyCode) const
{
	return App->input->GetKey(keyCode) == KEY_UP;
}

bool Scripting::KeyRepeat(int keyCode) const
{
	return App->input->GetKey(keyCode) == KEY_REPEAT;
}

void Scripting::GetMouseRaycast(float& x, float& y, float& z) const
{
	float3 hit;
	if (App->camera->MousePick(&hit) != nullptr) {
		x = hit.x;
		y = hit.y;
		z = hit.z;
	}
}

// OBJECT TRANSLATOR
void Scripting::CreateObj()
{

}

void Scripting::DestroyObj()
{

}

// Position
float Scripting::GetObjPosX(ScriptInstance* script)
{
	return ((ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->position.x;
}

float Scripting::GetObjPosY(ScriptInstance* script)
{
	return ((ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->position.y;
}

float Scripting::GetObjPosZ(ScriptInstance* script)
{
	return ((ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->position.z;
}

void Scripting::GetObjPos(ScriptInstance* script, float& x, float& y, float& z)
{
	ComponentTransform* trs = (ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	x = trs->position.x;
	y = trs->position.y;
	z = trs->position.z;
}

void Scripting::MoveObj(ScriptInstance* script, float x, float y, float z)
{
	ComponentTransform* trs = (ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->position += float3(x, y, z);
	trs->needsUpdateGlobal = true;
}

void Scripting::SetObjPos(ScriptInstance* script, float x, float y, float z)
{
	ComponentTransform* trs = (ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->position = float3(x, y, z);
	trs->needsUpdateGlobal = true;
}

// Rotation
float Scripting::GetObjRotX(ScriptInstance* script)
{
	return ((ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.x;
}

float Scripting::GetObjRotY(ScriptInstance* script)
{
	return ((ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.y;
}

float Scripting::GetObjRotZ(ScriptInstance* script)
{
	return ((ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.z;
}

void Scripting::GetObjRot(ScriptInstance* script, float& x, float& y, float& z)
{
	ComponentTransform* trs = (ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	
	x = trs->rotation.x;
	y = trs->rotation.y;
	z = trs->rotation.z;
}

void Scripting::RotateObj(ScriptInstance* script, float x, float y, float z)
{
	ComponentTransform* trs = (ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->rotation += float3(x, y, z);
	trs->needsUpdateGlobal = true;
}

void Scripting::SetObjRot(ScriptInstance* script, float x, float y, float z)
{
	ComponentTransform* trs = (ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->rotation += float3(x, y, z);
	trs->needsUpdateGlobal = true;
}

// Others
void Scripting::LookAt(ScriptInstance* script, float spotX, float spotY, float spotZ)
{
	ComponentTransform* trs = (ComponentTransform*)script->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->localTrs * float4x4::LookAt(trs->localTrs.Col3(2), float3(spotX, spotY, spotZ) - trs->position, trs->localTrs.Col3(1), float3::unitY);
	trs->needsUpdateGlobal = true;
}