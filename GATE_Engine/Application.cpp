#include "Application.h"
#include "libs/Brofiler/Brofiler.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	engineGUI = new ModuleEngineGUI(this);
	physics = new ModulePhysics(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(physics);

	//"Scene" (Currently the axis + white grid plane)
	AddModule(scene_intro);
	
	//Engine UI
	AddModule(engineGUI);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while (item != list_modules.rend())
	{
		delete *item;
		item++;
	}
}

bool Application::Init()
{
	bool ret = true;

	//Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->Init();
		item++;
	}

	//Collect hardware info
	hardware.CPU_logic_cores = SDL_GetCPUCount();
	hardware.RAM = (float)SDL_GetSystemRAM() / 1024;
	SDL_GetVersion(&hardware.sdl_version);

	//GPU
	hardware.GPU.vendor = (unsigned char*)glGetString(GL_VENDOR);
	hardware.GPU.renderer = (unsigned char*)glGetString(GL_RENDERER);
	hardware.GPU.version = (unsigned char*)glGetString(GL_VERSION);

	return ret;
}

// ---------------------------------------------

bool Application::Start()
{
	bool ret = true;

	std::list<Module*>::iterator item = list_modules.begin();

	//Call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->Start();
		item++;
	}
	
	frame_time.Start();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while (item != list_modules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}
	return ret;
}

// ---------------------------------------------

// Called before each update
void Application::PrepareUpdate()
{
	BROFILER_CATEGORY("App Prepare Update", Profiler::Color::DarkRed);

	frame_count++;
	last_sec_frame_count++;

	dt = (float)frame_time.ReadSec();
	frame_time.Start();

	//Update Hardware info such as VRAM usage
	GLint nTotalMemoryInKB = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &hardware.GPU.VRAM.available); // Total Available Memory in KB
	//hardware.GPU.VRAM.available / 1024; // Convert to MB

	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &hardware.GPU.VRAM.usage); // Dedicated VRAM in KB
	//hardware.GPU.VRAM.usage /= 1024; // Convert to MB

	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &hardware.GPU.VRAM.budget); // Total VRAM Memory in KB
	//hardware.GPU.VRAM.budget /= 1024; // Convert to MB
}

// Called each loop iteration
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	if (ret == true)
		ret = PreUpdateModules();

	if (ret == true)
		ret = UpdateModules();

	if (ret == true)
		ret = PostUpdateModules();

	if (input->GetWindowEvent(WE_QUIT) == true || mustShutDown)
		ret = UPDATE_STOP;

	FinishUpdate();

	return ret;
}

// Called after each update
void Application::FinishUpdate()
{
	BROFILER_CATEGORY("App Finish Update", Profiler::Color::IndianRed);

	//Framerate Calcs
	if (last_sec_frame_time.Read() > 1000) {

		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	//For performance information purposes
	float avg_fps = float(frame_count) / time_since_start.ReadSec();
	float secs_since_start = time_since_start.ReadSec();
	Uint32 last_frame_ms = frame_time.Read();
	Uint32 frames_on_last_update = prev_last_sec_frame_count;

	BROFILER_CATEGORY("App Delay", Profiler::Color::Gray);

	if (capped_ms > 0 && last_frame_ms < capped_ms)
		SDL_Delay(capped_ms - last_frame_ms);

	/*if (map->mapDebugDraw) {
		std::string tmp = std::to_string(prev_last_sec_frame_count);
		gui->fpsText->ChangeString(tmp);
	}*/
}

// ---------------------------------------------

// PreUpdate all modules in App
update_status Application::PreUpdateModules()
{
	BROFILER_CATEGORY("Modules Pre-Update", Profiler::Color::PaleVioletRed);

	update_status ret = UPDATE_CONTINUE;
	Module* pModule = NULL;
	std::list<Module*>::iterator item = list_modules.begin();

	for (; item != list_modules.end() && ret == UPDATE_CONTINUE; item = next(item)) {

		pModule = (*item);
		if (pModule->active == false)
			continue;

		ret = (*item)->PreUpdate(dt);
	}

	return ret;
}

// Update all modules in App
update_status Application::UpdateModules()
{
	BROFILER_CATEGORY("Modules Update", Profiler::Color::OrangeRed);

	update_status ret = UPDATE_CONTINUE;
	Module* pModule = NULL;

	std::list<Module*>::iterator item = list_modules.begin();
	for (; item != list_modules.end() && ret == UPDATE_CONTINUE; item = next(item)) {

		pModule = *item;
		if (pModule->active == false)
			continue;

		ret = (*item)->Update(dt);
	}

	return ret;
}

// PostUpdate all modules in App
update_status Application::PostUpdateModules()
{
	BROFILER_CATEGORY("Modules Post-Update", Profiler::Color::MediumVioletRed);

	update_status ret = UPDATE_CONTINUE;
	Module* pModule = NULL;
	std::list<Module*>::iterator item = list_modules.begin();

	for (; item != list_modules.end() && ret == UPDATE_CONTINUE; item = next(item)) {

		pModule = *item;
		if (pModule->active == false)
			continue;

		ret = (*item)->PostUpdate(dt);
	}

	return ret;
}

//Adding a module
void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}