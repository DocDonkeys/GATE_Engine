#ifndef __MODULESCRIPTING_H__
#define __MODULESCRIPTING_H__

#include "Module.h"
#include <vector>

class lua_State;
class ComponentScript;
struct ScriptFile;
struct ScriptInstance;

class ModuleScripting : public Module
{
public:
	ModuleScripting(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleScripting();

	void CompileScriptTableClass(ScriptInstance* script);
	void SendScriptToModule(ComponentScript* script_component, std::string full_file_path);
	ScriptFile* AddScriptFile(ComponentScript* script_component, std::string full_file_path);

public:
	bool Init();
	bool Start();
	bool CleanUp();
	update_status Update(float dt);

private:
	// L is our Lua Virtual Machine, it's called L because its the common name it receives, so all programers can understand what this var is
	lua_State *L = nullptr;
	bool start = true;

	std::vector<ScriptFile*> script_files;
	std::vector<ScriptInstance*> class_instances;
};


class Scripting
{
public:
	Scripting();
	~Scripting();

public:
	// ENGINE TRANSLATOR
	// General
	void LogFromLua(const char* string);
	void TestFunc();

	//void GetRealTime(luabridge::LuaRef* target);
	//void GetGameTime(luabridge::LuaRef* target);

	//// Input
	//void GetKey();
	//void GetMouseRaycastX(luabridge::LuaRef* target);
	//void GetMouseRaycastY(luabridge::LuaRef* target);
	//void GetMouseRaycastZ(luabridge::LuaRef* target);

	//// OBJECT TRANSLATOR
	//void CreateObj();
	//void DestroyObj();

	//// Position
	//void GetObjPosX(luabridge::LuaRef* target);
	//void GetObjPosY(luabridge::LuaRef* target);
	//void GetObjPosZ(luabridge::LuaRef* target);
	//void GetObjPos(luabridge::LuaRef* target);

	//void MoveObj();
	//void SetObjPos();

	//// Rotation
	//void GetObjRotX(luabridge::LuaRef* target);	// Roll
	//void GetObjRotY(luabridge::LuaRef* target);	// Pitch
	//void GetObjRotZ(luabridge::LuaRef* target);	// Yaw
	//void GetObjRot(luabridge::LuaRef* target);

	//void RotateObj();
	//void SetObjRot();

	//// Others
	//void LookAt(luabridge::LuaRef* target);
};


#endif // !__MODULESCRIPTING_H__

