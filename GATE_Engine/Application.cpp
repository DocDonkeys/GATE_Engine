#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this, "Window");
	input = new ModuleInput(this, "Input");
	scene_intro = new ModuleSceneIntro(this, "Scene");
	renderer3D = new ModuleRenderer3D(this, "Renderer");
	camera = new ModuleCamera3D(this, "Camera");
	engineGUI = new ModuleEngineGUI(this, "EngineUI");
	physics = new ModulePhysics(this, "Physics");

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

	//Load Settings before initializing modules
	configName.assign("settings/config.json");	//TODO: If config is deleted code breaks, add failsave method that creates new config with default values
	LoadConfig();

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

	// Get all CPU features
	if (SDL_Has3DNow())
		CPU_features.push_back("3DNow");

	if (SDL_HasAVX())
		CPU_features.push_back("AVX");

	if (SDL_HasAVX2())
		CPU_features.push_back("AVX2");

	if (SDL_HasAltiVec())
		CPU_features.push_back("AltiVec");

	if (SDL_HasMMX())
		CPU_features.push_back("MMX");

	if (SDL_HasRDTSC())
		CPU_features.push_back("RDTSC");

	if (SDL_HasSSE())
		CPU_features.push_back("SSE");

	if (SDL_HasSSE2())
		CPU_features.push_back("SSE2");

	if (SDL_HasSSE3())
		CPU_features.push_back("SSE3");

	if (SDL_HasSSE41())
		CPU_features.push_back("SSE41");

	if (SDL_HasSSE42())
		CPU_features.push_back("SSE42");
	// Finished collecting CPU features

	Start();
	return ret;
}

// ---------------------------------------------

bool Application::Start()
{
	bool ret = true;

	std::list<Module*>::iterator item = list_modules.begin();

	//Call Start() in all modules
	ConsoleLOG("Application Start --------------");
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
void Application::FinishUpdate()	//TODO: Separate in functions (Save&Load, Framerate Calculations, Hardware display update)
{
	BROFILER_CATEGORY("App Finish Update", Profiler::Color::IndianRed);

	//Save
	if (config_save == true) {
		SaveConfig();
		config_save = false;
	}

	//Load
	if (config_load == true) {
		LoadConfig();
		config_load = false;
	}
	
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

	// Update the fps Log
	fps_log.push_back(prev_last_sec_frame_count);
	if (fps_log.size() > 100)
	{
		fps_log.erase(fps_log.begin());
	}

	// Update the ms Log
	ms_log.push_back(last_frame_ms);
	if (ms_log.size() > 100)
	{
		ms_log.erase(ms_log.begin());
	}

	if (max_FPS > 0)
		capped_ms = 1000 / max_FPS;
	else
		capped_ms = -1;

	if (capped_ms > 0 && last_frame_ms < capped_ms)
		SDL_Delay(capped_ms - last_frame_ms);

	/*if (map->mapDebugDraw) {
		std::string tmp = std::to_string(prev_last_sec_frame_count);
		gui->fpsText->ChangeString(tmp);
	}*/
}

// ---------------------------------------------

void Application::RequestBrowser(const char* link)
{
	ShellExecuteA(NULL, "open", link, NULL, NULL, SW_SHOWNORMAL);
}

void Application::ConsoleLOG(const char * format, ...)
{

	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "%s", tmp_string);
	OutputDebugString(tmp_string2);
	// First we do  a normal LOG to VS using a modified LOG that just returns the LOG message
	std::string str = tmp_string2;
	
	
	//Now we can pass the string to the vector
	console_LOG.push_back(str);
}

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
	BROFILER_CATEGORY("Modules Update", Profiler::Color::Crimson);

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

// Adding a module
void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

// Get App data
const char* Application::GetTitle() const
{
	return title.data();
}

const char* Application::GetOrganization() const
{
	return organization.data();
}

// ---------------------------------------
float Application::GetDT() const
{
	return dt;
}

// Save / Load Requests
void Application::RequestLoad(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	projFilename.assign(file);
	want_to_load = true;
}

void Application::RequestSave(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?
	projFilename.assign(file);
	want_to_save = true;
}

void Application::RequestConfigLoad()
{
	config_load = true;
}

void Application::RequestConfigSave() const
{
	config_save = true;
}

// Save / Load Operations
bool Application::LoadProject(const char* file)
{
	bool ret = true;

	return ret;
}

bool Application::SaveProject(const char* file) const
{
	bool ret = true;

	return ret;
}

bool Application::LoadConfig()
{
	bool ret = true;

	// --- Load App data from JSON files ---
	json config = jLoad.Load("Settings/config.json");

	//App
	title = config["App"]["Title"].value;
	organization = config["App"]["Organization"].value;

	//Window
	window->window_width = config["Window"]["Width"].value;
	window->window_height = config["Window"]["Height"].value;
	window->window_fullscreen = config["Window"]["Fullscreen"].value;
	window->window_resizable = config["Window"]["Resizable"].value;
	window->window_borderless = config["Window"]["Borderless"].value;
	window->window_full_desktop = config["Window"]["FullscreenDesktop"].value;

	//Input

	//Renderer
	renderer3D->vSync = config["Renderer3D"]["VSync"].value;

	//GUI
	engineGUI->show_demo_window = config["GUI"]["DemoWindow"].value;
	engineGUI->show_another_window = config["GUI"]["AnotherWindow"].value;
	engineGUI->show_configuration_window = config["GUI"]["ConfigurationWindow"].value;
	engineGUI->show_console_window = config["GUI"]["ConsoleWindow"].value;

	return ret;
}

bool Application::SaveConfig() const
{
	bool ret = true;

	//Create JSON obj with imported values
	json config = {
		{"App", {
			{"Title", title.c_str()},
			{"Organization", organization.c_str()}
		}},

		{"Window", {
			{"Width", window->window_width},
			{"Height", window->window_height},
			{"Fullscreen", window->window_fullscreen},
			{"Resizable", window->window_resizable},
			{"Borderless", window->window_borderless},
			{"FullscreenDesktop", window->window_full_desktop}
		}},

		{"Input", {

		}},

		{"Renderer3D", {
			{"VSync", renderer3D->vSync}
		}},

		{"GUI", {
			{"DemoWindow", engineGUI->show_demo_window},
			{"AnotherWindow", engineGUI->show_another_window},
			{"ConfigurationWindow", engineGUI->show_configuration_window},
			{"ConsoleWindow", engineGUI->show_console_window},
		}},
	};

	jLoad.Save(config, configName.c_str());

	return ret;
}