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
#include "ScriptVar.h"
#include "MemLeaks.h"
#include <iterator>

#include "libs/MathGeoLib/include/Math/float3x3.h"
#include "libs/MathGeoLib/include/Math/float4x4.h"
#include "libs/MathGeoLib/include/Math/MathFunc.h"
#include "libs/SDL/include/SDL_keyboard.h"

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

bool ModuleScripting::DoHotReloading()
{
	bool ret = true;

	if (App->scene_intro->playing == false)
	{
		//We do the necessary Hot Reloading
		for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
		{
			//Remove the references to the data inside the virtual machine
			(*it)->my_table_class = 0;

			//Copy the necessary data into new instances which will hold the new compiled tables
			ScriptInstance* recompiled_instance = new ScriptInstance;

			recompiled_instance->my_component = (*it)->my_component;
			recompiled_instance->my_resource = (*it)->my_resource;

			recompiled_instances.push_back(recompiled_instance);
		}
		CleanUpInstances();
		//Close the virtual machine & Destroy it
		lua_close(L);

		//Create the new Virtual Machine
		L = luaL_newstate();
		luaL_openlibs(L);

		//Acquire All scripts to be compiled       (we will compile even scripts which are currently not attached to any gameobject)
		//to check if it still compiles after the change done in a given script which is unknown to us
		std::string extension = "lua";
		std::vector<std::string> files;
		App->resources->GetAllFilesWithExtension(extension,files,App->resources->assets_dir);

		bool can_instantiate_scripts = true;
		//Compile all the scripts of the Engine
		for (int i = 0; i < files.size(); ++i)
		{
			if (JustCompile(files[i]) == false)
			{
				can_instantiate_scripts = false;
				ret = false;
				LOG("[Warning] Fix all compiler Errors!");
				cannot_start = true;
			}
		}

		if (can_instantiate_scripts == true)
		{
			//If everything compiled just fine, give the recompiled instances the new version of the script
			for (std::vector<ScriptInstance*>::iterator it = recompiled_instances.begin(); it != recompiled_instances.end(); ++it)
				CompileScriptTableClass((*it));

			for (int i = 0; i < recompiled_instances.size(); ++i)
				class_instances.push_back(recompiled_instances[i]);

			recompiled_instances.clear();
		}

		hot_reloading_waiting = false;
	}
	else
	{
		hot_reloading_waiting = true;
	}

	if (ret == true)
		cannot_start = false;

	return ret;
}

//This function just compiles a script and checks if it compiled or it prompts the error to console
bool ModuleScripting::JustCompile(std::string relative_path)
{
	bool ret = false;

	luabridge::getGlobalNamespace(L)
		.beginNamespace("Debug")
		.beginClass <Scripting>("Scripting")
		.addConstructor<void(*) (void)>()
		.addFunction("LOG", &Scripting::LogFromLua)
		.endClass()
		.endNamespace();

	Scripting Scripting;

	std::string absolute_path = App->file_system->GetPathToGameFolder(true) + relative_path;
	int compiled = luaL_dofile(L, absolute_path.c_str());

	if (compiled == LUA_OK)
	{
		//We don't need to do nothing here, LOG something at most
		LOG("Compiled %s successfully!", relative_path);
		ret = true;
	}
	else
	{
		std::string error = lua_tostring(L, -1);
		LOG("%s", error.data());
	}

	return ret;
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

				for (luabridge::Iterator iterator(script->my_table_class); !iterator.isNil(); ++iterator)
				{
					// Use iterator.key () and iterator.value() here
					bool is_number = (*iterator).first.isNumber();
					bool is_string = (*iterator).first.isString();
					bool is_function = (*iterator).first.isFunction();
					bool is_bool = (*iterator).first.isBool();


					bool value_is_number = (*iterator).second.isNumber();
					bool value_is_string = (*iterator).second.isString();
					bool value_is_function = (*iterator).second.isFunction();
					bool value_is_bool = (*iterator).second.isBool();

					std::string str = (*iterator).first.tostring();

					ScriptVar var = ScriptVar(true);

					int test = 0;
				}
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

update_status ModuleScripting::Update(float realDT)
{
	//If a script was changed during runtime, hot reload
	if (!App->IsGamePlaying() && hot_reloading_waiting)
		DoHotReloading();

	// Carles to Didac
	// 1. You can use the "IsWhatever" functions of App to check the current game state.
	// 2. "App->IsGameFirstFrame()" marks the first frame a GameUpdate() will happen, if you want to do anything right before the game plays in preparation
	// 3. Referring to your previous code, you expected DoHotReloading() to NOT run if the game is playing, I put the condition accordingly "!IsGamePlaying()"



	//TEST FUNCTION DEFINETIVELY SHOULD NOT BE HERE
	if (App->scene_intro->selected_go != nullptr && App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
		GameObject* returned = GOFunctions::InstantiateGameObject(App->scene_intro->selected_go);
	return UPDATE_CONTINUE;
}

update_status ModuleScripting::GameUpdate(float gameDT)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Debug")
		.beginClass <Scripting>("Scripting")
		.addConstructor<void(*) (void)>()

		// General
		.addFunction("LOG", &Scripting::LogFromLua)
		.addFunction("time", &Scripting::GetTime)
		.addFunction("dt", &Scripting::GetDT)

		// Input
		.addFunction("KeyState", &Scripting::GetKeyState)
		.addFunction("KeyDown", &Scripting::IsKeyDown)
		.addFunction("KeyUp", &Scripting::IsKeyUp)
		.addFunction("KeyRepeat", &Scripting::IsKeyRepeat)
		.addCFunction("GetMouseRaycastHit", &Scripting::GetMouseRaycastHit)

		// Script Data
		.addFunction("Enable", &Scripting::Enable)
		.addFunction("enabled", &Scripting::IsEnabled)

		// Object Data
		//.addFunction("gameObject", &Scripting::GetGameObject)
		//.addFunction("GO_name", &Scripting::GetObjectName)
		.addFunction("GO_Enable", &Scripting::ActivateObject)
		.addFunction("go_enabled", &Scripting::IsObjectActivated)
		.addFunction("Destroy", &Scripting::DestroySelf)

		// Transform Position
		.addFunction("position_x", &Scripting::GetPositionX)
		.addFunction("position_y", &Scripting::GetPositionY)
		.addFunction("position_z", &Scripting::GetPositionZ)
		.addFunction("position", &Scripting::GetPosition)
		.addFunction("Translate", &Scripting::Translate)
		.addFunction("SetPosition", &Scripting::SetPosition)

		// Transform Rotation
		.addFunction("rotation_x", &Scripting::GetEulerX)
		.addFunction("rotation_y", &Scripting::GetEulerY)
		.addFunction("rotation_z", &Scripting::GetEulerZ)
		.addFunction("rotation", &Scripting::GetEulerRotation)
		.addFunction("Rotate", &Scripting::Rotate)
		.addFunction("SetRotation", &Scripting::SetEulerRotation)

		// Uility
		.addFunction("LookAt", &Scripting::LookAt)
		.addFunction("LookTo", &Scripting::LookTo)

		.endClass()
		.endNamespace();

	Scripting Scripting;
	//Building a class / Namespace so Lua can have this object to Call EngineLOG by calling
	if (cannot_start == false && App->scene_intro->playing == true)
	{
		for (current_script = class_instances.begin(); current_script != class_instances.end(); ++current_script)
		{
			if ((*current_script)->awoken == false)
			{
				(*current_script)->my_table_class["Awake"]();	// Awake is done first, regardless of the script being active or not
				(*current_script)->awoken = true;
			}
			else if ((*current_script)->my_component->active == true) //Check if the script instance is active or not
			{
				if ((*current_script)->started == false)
				{
					(*current_script)->my_table_class["Start"]();	// Start is done only once for the first time the script is active
					(*current_script)->started = true;
				}
				else
				{
					(*current_script)->my_table_class["Update"]();	// Update is done on every iteration of the script as long as it remains active
				}
		}
	}
	}
	return UPDATE_CONTINUE;
}


void ModuleScripting::CleanUpInstances()
{
	for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
	{
		RELEASE((*it));
	}

	class_instances.clear();
}


void ModuleScripting::Stop()
{
	for (std::vector<ScriptInstance*>::iterator it = class_instances.begin(); it != class_instances.end(); ++it)
		(*it)->awoken = (*it)->started = false;
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
	LOG("[Script]: %s", string);
}

void Scripting::TestFunc()
{
	LOG("This is a test function that Logs something (TestFunc)");
}

uint Scripting::GetRealTime() const
{
	return App->GetRealTime();
}

uint Scripting::GetTime() const
{
	return App->GetGameTime();
}

float Scripting::GetRealDT() const
{
	return App->GetRealDT();
}

float Scripting::GetDT() const
{
	return App->GetGameDT();
}

// Input
int Scripting::GetKeyState(const char* key) const
{
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN)
		return App->input->GetKey(code);
	else
		return -1;
}

bool Scripting::IsKeyDown(const char* key) const
{
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN)
		return App->input->GetKey(code) == KEY_DOWN;
	else
		return false;
}

bool Scripting::IsKeyUp(const char* key) const
{
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN)
		return App->input->GetKey(code) == KEY_UP;
	else
		return false;
}

bool Scripting::IsKeyRepeat(const char* key) const
{
	SDL_Scancode code = SDL_GetScancodeFromName(key);
	if (code != SDL_SCANCODE_UNKNOWN)
		return App->input->GetKey(code) == KEY_REPEAT;
	else
		return false;
}

int Scripting::GetMouseRaycastHit(lua_State *L)
{
	float3 hit = float3::zero;
	const GameObject* go = App->camera->MousePick(&hit);

	if (go != nullptr) {
		lua_pushnumber(L, hit.x);
		lua_pushnumber(L, hit.y);
		lua_pushnumber(L, hit.z);
	}

	return 3;
}

// GameObjects
//const GameObject* Scripting::Find(const char* objName) const
//{
//	const GameObject* ret = nullptr;
//
//	std::vector<const GameObject*> vec;
//	GOFunctions::FillArrayWithChildren(vec, App->scene_intro->root);
//	for (std::vector<const GameObject*>::iterator it = vec.begin(); it != vec.end(); ++it)
//		if ((*it)->name == objName) {
//			ret = (*it);
//			break;
//		}
//
//	return ret;
//}

void Scripting::Enable(bool state)
{
	(*App->scripting->current_script)->my_component->active = state;
}

bool Scripting::IsEnabled() const
{
	return (*App->scripting->current_script)->my_component->active;
}

// OBJECT TRANSLATOR

// General
const GameObject* Scripting::GetGameObject() const
{
	return (*App->scripting->current_script)->my_component->my_go;
}

const char* Scripting::GetObjectName() const
{
	return (*App->scripting->current_script)->my_component->my_go->name.c_str();
}

void Scripting::ActivateObject(bool state)
{
	(*App->scripting->current_script)->my_component->my_go->UpdateChildrenActive(state);
}

bool Scripting::IsObjectActivated() const
{
	return (*App->scripting->current_script)->my_component->my_go->active;
}

void Scripting::SetStatic(bool state)
{
	(*App->scripting->current_script)->my_component->my_go->UpdateStaticStatus(state);
}

bool Scripting::IsStatic() const
{
	return (*App->scripting->current_script)->my_component->my_go->staticObj;
}

void Scripting::DestroySelf() const
{
	App->scene_intro->DestroyGameObject((*App->scripting->current_script)->my_component->my_go);
}

// Position
float Scripting::GetPositionX(bool local) const
{
	if (local)
		return ((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->position.x;
	else
		return ((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.TranslatePart().x;
}

float Scripting::GetPositionY(bool local) const
{
	if (local)
		return ((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->position.y;
	else
		return ((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.TranslatePart().y;
}

float Scripting::GetPositionZ(bool local) const
{
	if (local)
		return ((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->position.z;
	else
		return ((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.TranslatePart().z;
}

int Scripting::GetPosition(bool local, lua_State *L) const
{
	ComponentTransform* trs = (ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	float3 pos;

	if (local)
		pos = trs->position;
	else
		pos = trs->globalTrs.TranslatePart();

	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	lua_pushnumber(L, pos.z);

	return 3;
}

void Scripting::Translate(float x, float y, float z, bool local)
{
	ComponentTransform* trs = (ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->Translate(float3(x, y, z), local);
}

void Scripting::SetPosition(float x, float y, float z, bool local)
{
	ComponentTransform* trs = (ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->SetPosition(float3(x, y, z), local);
}

// Rotation
float Scripting::GetEulerX(bool local) const
{
	if (local)
		return RadToDeg(((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.x);
	else
		return RadToDeg(((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().ToEulerXYZ().x);
}

float Scripting::GetEulerY(bool local) const
{
	if (local)
		return RadToDeg(((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.y);
	else
		return RadToDeg(((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().ToEulerXYZ().y);
}

float Scripting::GetEulerZ(bool local) const
{
	if (local)
		return RadToDeg(((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->rotation.z);
	else
		return RadToDeg(((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM))->globalTrs.RotatePart().ToEulerXYZ().z);
}

int Scripting::GetEulerRotation(bool local, lua_State *L) const
{
	ComponentTransform* trs = (ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	float3 rot;

	if (local)
		rot = RadToDeg(trs->rotation);
	else
		rot = RadToDeg(trs->globalTrs.RotatePart().ToEulerXYZ());

	lua_pushnumber(L, rot.x);
	lua_pushnumber(L, rot.y);
	lua_pushnumber(L, rot.z);

	return 3;
}

void Scripting::Rotate(float x, float y, float z, bool local)
{
	ComponentTransform* trs = (ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->Rotate(DegToRad(float3(x, y, z)), local);
}

void Scripting::SetEulerRotation(float x, float y, float z, bool local)
{
	ComponentTransform* trs = (ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	trs->SetRotation(DegToRad(float3(x, y, z)), local);
}

// Others
void Scripting::LookAt(float spotX, float spotY, float spotZ, bool local)
{
	ComponentTransform* trs = (ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);
	float3 dir;

	if (local)
		dir = (float3(spotX, spotY, spotZ) - trs->localTrs.TranslatePart());
	else
		dir = (float3(spotX, spotY, spotZ) - trs->globalTrs.TranslatePart());

	LookTo(dir.x, dir.y, dir.z, local);
}

void Scripting::LookTo(float dirX, float dirY, float dirZ, bool local)
{
	ComponentTransform* trs = (ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->GetComponent(COMPONENT_TYPE::TRANSFORM);

	if (local)
		trs->localTrs = trs->localTrs * float4x4::LookAt(trs->localTrs.WorldZ(), float3(dirX, dirY, dirZ).Normalized(), trs->localTrs.WorldY(), float3::unitY);
	else {
		trs->localTrs = trs->localTrs * float4x4::LookAt(trs->localTrs.WorldZ(), float3(dirX, dirY, dirZ).Normalized(), trs->localTrs.WorldY(), float3::unitY);
		trs->localTrs = ((ComponentTransform*)(*App->scripting->current_script)->my_component->my_go->parent->GetComponent(COMPONENT_TYPE::TRANSFORM))->localTrs.Inverted().RotatePart() * trs->localTrs;	// Maybe there's a simpler way, but this works and by God I'm not fucking touching it again for now
	}

	trs->needsUpdateGlobal = true;
}