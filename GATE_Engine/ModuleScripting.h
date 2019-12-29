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

public:
	bool DoHotReloading();
	bool JustCompile(std::string relative_path);
	void CompileScriptTableClass(ScriptInstance* script);
	void SendScriptToModule(ComponentScript* script_component, std::string full_file_path);
	ScriptFile* AddScriptFile(ComponentScript* script_component, std::string full_file_path);
	void FillScriptInstanceComponentVars(ScriptInstance* script);
	void DeleteScriptInstanceWithParentComponent(ComponentScript* script_component);

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
	bool start = true;
	bool cannot_start = false; //We cannot start if a compilation error would cause a crash on the engine when we start playing
	bool hot_reloading_waiting = false;

	void CleanUpInstances();

	std::vector<ScriptFile*> script_files;
	std::vector<ScriptInstance*> recompiled_instances;
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
	int GetKey(const char* key) const;
	int GetKeyState(const char* key) const;

	bool IsKeyDown(const char* key) const;
	bool IsKeyUp(const char* key) const;
	bool IsKeyRepeat(const char* key) const;
	bool IsKeyIdle(const char* key) const;

	int GetMouseButton(const char* button) const;
	int GetMouseButtonState(const char* button) const;

	bool IsMouseButtonDown(const char* button) const;
	bool IsMouseButtonUp(const char* button) const;
	bool IsMouseButtonRepeat(const char* button) const;
	bool IsMouseButtonIdle(const char* button) const;

	int GetMouseRaycastHit(lua_State *L);

	// GameObjects
	GameObject* FindGameObject(const char* objName) const;
	uint32_t FindGameObjectUID(const char* objName) const;
	GameObject* Instantiate(GameObject* reference, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);
	uint32_t InstantiateByUID(uint32_t objUID, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);
	GameObject* InstantiateByName(const char* objName, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale);
	GameObject* Destroy(GameObject* target);

	// SCRIPT TRANSLATOR
	void Enable(bool state);
	bool IsEnabled() const;

	// OBJECT TRANSLATOR
	// General
	const GameObject* GetGameObject() const;
	const char* GetObjectName() const;

	void ActivateObject(bool state);
	bool IsObjectActivated() const;

	void SetStatic(bool state);
	bool IsStatic() const;

	void DestroySelf() const;

	// Position
	float GetPositionX(bool local) const;
	float GetPositionY(bool local) const;
	float GetPositionZ(bool local) const;
	int GetPosition(bool local, lua_State *L) const;

	void Translate(float x, float y, float z, bool local);
	void SetPosition(float x, float y, float z, bool local);

	// Rotation
	float GetEulerX(bool local) const;	// Roll
	float GetEulerY(bool local) const;	// Pitch
	float GetEulerZ(bool local) const;	// Yaw
	int GetEulerRotation(bool local, lua_State *L) const;
	
	float GetQuatX(bool local) const;
	float GetQuatY(bool local) const;
	float GetQuatZ(bool local) const;
	float GetQuatW(bool local) const;
	int GetQuatRotation(bool local, lua_State *L) const;

	void Rotate(float x, float y, float z, bool local);
	void SetEulerRotation(float x, float y, float z, bool local);

	// Others
	void LookAt(float posX, float posY, float posZ, bool local);
	void LookTo(float dirX, float dirY, float dirZ, bool local);
};

#endif // !__MODULESCRIPTING_H__
