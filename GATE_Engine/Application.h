#ifndef APP_H
#define APP_H

// Libraries
#include <list>
#include <string>
#include <vector>

// Headers
#include "Globals.h"
#include "Timer.h"
#include "RNGenerator.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEngineGUI.h"
#include "ModulePhysics.h"

// SDL
#include "libs/SDL/include/SDL.h"
#include "libs/SDL/include/SDL_version.h"

// JSON
#include "libs/parson/parson.h"

struct GPU_info
{
	struct VRAM_info
	{
		int budget, usage, available;
	};

	unsigned char* vendor = nullptr;
	unsigned char* renderer = nullptr;
	unsigned char* version = nullptr;

	VRAM_info VRAM;
};

struct hardware_info  //Struct to keep data more organized and not add a lot of vars in the class
{
	SDL_version sdl_version;

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
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	float GetDT() const;

	// Load / Save Requests
	void RequestLoad();
	void RequestSave() const;

	void RequestBrowser(const char* link);

private:
	// Call module cycle phases in order on App Update() phase
	update_status PreUpdateModules();
	update_status UpdateModules();
	update_status PostUpdateModules();

	void AddModule(Module* mod);

	// Load / Save Operations
	bool LoadFile();
	bool SaveFile() const;

public:
	//Modules
	ModuleWindow*		window;
	ModuleInput*		input;
	ModuleSceneIntro*	scene_intro;
	ModuleRenderer3D*	renderer3D;
	ModuleCamera3D*		camera;
	ModuleEngineGUI*	engineGUI;
	ModulePhysics*		physics;

	//App members
	hardware_info		hardware;
	RNGenerator			rng;
	bool				mustShutDown = false;

	std::vector<float>	ms_log;
	std::vector<float>	fps_log;
	int					max_FPS = -1;

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
	std::string			title;
	std::string			organization;

	//Save & Load
	mutable bool		want_to_save;
	bool				want_to_load;

	//Modules
	std::list <Module*> list_modules;

};

#endif //APP_H