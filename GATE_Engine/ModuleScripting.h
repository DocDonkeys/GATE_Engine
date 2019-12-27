#ifndef __MODULESCRIPTING_H__
#define __MODULESCRIPTING_H__

#include "Module.h"
#include <vector>
#include "ImporterScript.h"

class lua_State;
class GameObject;
class ComponentScript;
struct ScriptFile;
struct ScriptInstance;

class ModuleScripting : public Module
{
public:
	ModuleScripting(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleScripting();

	void DoHotReloading();
	void CompileScriptTableClass(ScriptInstance* script);
	void SendScriptToModule(ComponentScript* script_component, std::string full_file_path);
	ScriptFile* AddScriptFile(ComponentScript* script_component, std::string full_file_path);

	//If a lua file doesn't have a meta
	void ManageOrphanScript(std::string relative_path);

public:
	bool Init();
	bool Start();
	bool CleanUp();

	update_status Update(float realDT);
	update_status GameUpdate(float gameDT);

	void Stop();

public:
	ImporterScript ie_scripts;
	std::vector<ScriptInstance*>::iterator current_script;

private:
	// L is our Lua Virtual Machine, it's called L because its the common name it receives, so all programers can understand what this var is
	lua_State *L = nullptr;
	bool hot_reloading_waiting = false;

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

	uint GetRealTime() const;
	uint GetTime() const;

	float GetRealDT() const;
	float GetDT() const;

	// Input
	int GetKeyState(const char* key) const;
	bool KeyDown(const char* key) const;
	bool KeyUp(const char* key) const;
	bool KeyRepeat(const char* key) const;

	void GetMouseRaycast(float& x, float& y, float& z) const;

	// GameObjects
	//const GameObject* Find(const char* objName) const;

	// OBJECT TRANSLATOR
	//void Instantiate();
	//void Destroy();

	// Position
	float GetPositionX();
	float GetPositionY();
	float GetPositionZ();
	void GetPosition(float& x, float& y, float& z);

	void Move(float x, float y, float z);
	void SetPosition(float x, float y, float z);

	// Rotation
	float GetEulerX();	// Roll
	float GetEulerY();	// Pitch
	float GetEulerZ();	// Yaw
	void GetEulerRotation(float& x, float& y, float& z);

	void Rotate(float x, float y, float z);
	void SetEulerRotation(float x, float y, float z);

	// Others
	void LookAt(float posX, float posY, float posZ);
};

#endif // !__MODULESCRIPTING_H__
