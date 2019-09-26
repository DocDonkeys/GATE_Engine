#include "Application.h"

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

	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->Init();
		item++;
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->Start();
		item++;
	}
	
	ms_timer.Start();

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
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();

	//Update Hardware info such as VRAM usage
	GLint nTotalMemoryInKB = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &hardware.GPU.VRAM.available); // Total Available Memory in KB
	//hardware.GPU.VRAM.available / 1024; // Convert to MB

	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &hardware.GPU.VRAM.usage); // Dedicated VRAM in KB
	//hardware.GPU.VRAM.usage /= 1024; // Convert to MB

	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &hardware.GPU.VRAM.budget); // Total VRAM Memory in KB
	//hardware.GPU.VRAM.budget /= 1024; // Convert to MB

	
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(dt);
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(dt);
		item++;
	}

	FinishUpdate();
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

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}