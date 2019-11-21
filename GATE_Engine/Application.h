#ifndef APP_H
#define APP_H

// Libraries
#include <list>
#include <vector>

// Headers
#include "Globals.h"
#include "Timer.h"
#include "RNGenerator.h"

// SDL
//#include "libs/SDL/include/SDL.h"
//#include "libs/SDL/include/SDL_version.h"

// JSON
#include "JsonLoader.h"

// Brofiler
//#include "libs/Brofiler/Brofiler.h"

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleSceneIntro;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleEditor;
class ModulePhysics;
class GeometryLoader;
class TextureLoader;
class ModuleFileSystem;
class ModuleResources;

struct GPU_info
{
	struct VRAM_info
	{
		float budget, usage, available;
	};

	unsigned char* vendor = nullptr;
	unsigned char* renderer = nullptr;
	unsigned char* version = nullptr;

	VRAM_info VRAM;
};

struct DevIL_info
{
	unsigned char* vendor = nullptr;
	unsigned char* version = nullptr;
};

struct hardware_info  //Struct to keep data more organized and not add a lot of vars in the class
{
	SDL_version sdl_version;
	DevIL_info devil_info;

	Uint8 CPU_logic_cores;
	float RAM;

	GPU_info GPU;
};

class Application
{
public:
	Application();
	~Application();

	// Called once on object creation
	bool Init();

	// When starting and quitting App
	bool Start();
	bool CleanUp();

	// App update loop
	void PrepareUpdate();
	update_status Update();
	void FinishUpdate();

public:
	// Exposing some properties for reading
	const char* GetTitle() const;
	const char* GetOrganization() const;
	const char* GetAuthors() const;
	const char* GetLicense() const;
	float GetDT() const;

	// Load / Save Requests
	void RequestLoad(const char* file);
	void RequestSave(const char* file) const;

	void RequestConfigSave() const;
	void RequestConfigLoad();
	void RequestConfigReset();

	//Open the Web Page provided by the link
	void RequestBrowser(const char* link);
	//LOG info to both Visual Studio & the editor / Engine UI console
	void ConsoleLOG(const char* format,...);

	//Subract a part of a string to obtain the desired characters of a path ex: filename, extension, path to a file etc.
	std::string SubtractString(std::string& str, const char* chars_to_find, bool reading_backwards, bool subtract_until_char, bool include_char = false);

private:
	// Call module cycle phases in order on App Update() phase
	update_status PreUpdateModules();
	update_status UpdateModules();
	update_status PostUpdateModules();

	void AddModule(Module* mod);

	// Grouped Operations
	void CheckFileEditRequests();
	void FramerateLogic();

	// Load / Save Operations
	bool LoadDefaultConfig();
	bool LoadUserConfig();
	
	bool LoadConfig(json& obj);
	bool SaveConfig() const;

	bool LoadProject(const char* file);
	bool SaveProject(const char* file) const;

public:
	//JSON
	JsonLoader jLoad;
	std::string defaultConfig;
	std::string editableConfig;

	//Modules
	ModuleWindow*		window;
	ModuleInput*		input;
	ModuleSceneIntro*	scene_intro;
	ModuleRenderer3D*	renderer3D;
	ModuleCamera3D*		camera;
	ModuleEditor*		editor;
	ModulePhysics*		physics;
	GeometryLoader*		geometry_loader;
	TextureLoader*		texture_loader;
	ModuleFileSystem*	file_system;
	ModuleResources*	resources;

	//App members
	hardware_info		hardware;
	RNGenerator			rng;
	bool				mustShutDown = false;

	std::vector<float>			ms_log;
	std::vector<float>			fps_log;
	std::vector<std::string>	LOG;
	std::vector<std::string>	CPU_features;
	int							max_FPS = -1;

private:
	//Framerate
	uint				frame_count = 0;
	Timer				time_since_start;
	Timer				frame_time;
	Timer				last_sec_frame_time;

	Uint32				last_sec_frame_count = 0;
	Uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = -1;

	//App data
	std::string	title;
	std::string	organization;
	std::string	authors;
	std::string	license;

	//Save & Load
	mutable std::string	projFilename;
	mutable bool		want_to_save;
	bool				want_to_load;
	
	mutable bool		config_save;
	bool				config_load;
	bool				config_reset;

	//Modules
	std::list <Module*> list_modules;

};

extern Application* App;

#endif //APP_H