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

	uint GetRealTime();
	uint GetGameTime();

	// Input
	int GetKeyState(int keyCode) const;
	bool KeyDown(int keyCode) const;
	bool KeyUp(int keyCode) const;
	bool KeyRepeat(int keyCode) const;

	void GetMouseRaycast(float& x, float& y, float& z) const;

	// OBJECT TRANSLATOR
	void CreateObj();
	void DestroyObj();

	// Position
	float GetObjPosX(ScriptInstance* script);
	float GetObjPosY(ScriptInstance* script);
	float GetObjPosZ(ScriptInstance* script);
	void GetObjPos(ScriptInstance* script, float& x, float& y, float& z);

	void MoveObj(ScriptInstance* script, float x, float y, float z);
	void SetObjPos(ScriptInstance* script, float x, float y, float z);

	// Rotation
	float GetObjRotX(ScriptInstance* script);	// Roll
	float GetObjRotY(ScriptInstance* script);	// Pitch
	float GetObjRotZ(ScriptInstance* script);	// Yaw
	void GetObjRot(ScriptInstance* script, float& x, float& y, float& z);

	void RotateObj(ScriptInstance* script, float x, float y, float z);
	void SetObjRot(ScriptInstance* script, float x, float y, float z);

	// Others
	void LookAt(ScriptInstance* script, float posX, float posY, float posZ);
};


#endif // !__MODULESCRIPTING_H__

