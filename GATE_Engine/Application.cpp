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

	//Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end())
	{
		(*item)->Init();
		item++;
	}

	//App data	//TODO: SAVE/LOAD
	title.append(TITLE);
	organization.append("DocDonkeys");

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
	if (want_to_save == true) {
		SaveFile();
		want_to_save = false;
	}

	//Load
	if (want_to_load == true) {
		LoadFile();
		want_to_load = false;
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

// Save / Load
void Application::RequestLoad()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
}

void Application::RequestSave() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?
	want_to_save = true;
}

//Example
/*
void persistence_example(void) {
	JSON_Value *schema = json_parse_string("{\"name\":\"\"}");
	JSON_Value *user_data = json_parse_file("user_data.json");
	char buf[256];
	const char *name = NULL;
	if (user_data == NULL || json_validate(schema, user_data) != JSONSuccess) {
		puts("Enter your name:");
		scanf("%s", buf);
		user_data = json_value_init_object();
		json_object_set_string(json_object(user_data), "name", buf);
		json_serialize_to_file(user_data, "user_data.json");
	}
	name = json_object_get_string(json_object(user_data), "name");
	printf("Hello, %s.", name);
	json_value_free(schema);
	json_value_free(user_data);
	return;
}
*/

bool Application::LoadFile()
{
	bool ret = false;

	//pugi::xml_document data;
	//pugi::xml_node root;

	//load_game = save_game;	// @Carles

	//pugi::xml_parse_result result = data.load_file(load_game.c_str());

	//if (result != NULL)
	//{
	//	LOG("Loading new Game State from %s...", load_game.c_str());

	//	root = data.child("game_state");

	//	std::list<Module*>::iterator item = modules.begin();
	//	ret = true;

	//	for (item; item != modules.end() && ret == true; item = next(item))
	//	{
	//		ret = (*item)->Load(root.child((*item)->name.c_str()));
	//	}

	//	data.reset();
	//	if (ret == true)
	//		LOG("...finished loading");
	//	else
	//		LOG("...loading process interrupted with error on module %s", (*item != NULL) ? (*item)->name.c_str() : "unknown");
	//}
	//else
	//	LOG("Could not parse game state xml file %s. pugi error: %s", load_game.c_str(), result.description());

	////Alternate Start Game that loads game after reading data
	//myApp->gui->DeactivateScreen(myApp->gui->Main_Menu_Elements);
	//myApp->gui->ActivateScreen(myApp->gui->InGame_Elements);

	//myApp->gui->hordeNumber_Label->ChangeString(std::to_string(myApp->hordes->roundNumber));
	//myApp->gui->MainMenuTemp_Image->Deactivate();

	//myApp->gui->Current_Screen = Screen_Type::SCREEN_INGAME;
	//myApp->scene->SwitchMusic(Screen_Type::SCREEN_INGAME);
	//Mix_Resume(-1);
	//myApp->scene->ActivateGameOverMusic = true;
	//myApp->gui->OnPause = false;
	//myApp->gui->WinIcon->Deactivate();
	//myApp->gui->LoseIcon->Deactivate();

	//myApp->video->StopVideo();

	return ret;
}

bool Application::SaveFile() const
{
	bool ret = true;

	//std::string save_game_file = save_game.c_str();	// @Carles, IMPROVE: Could work with a list of saved files instead of having just one

	//LOG("Saving Game State to %s...", save_game_file.c_str());

	//// xml object were we will store all data
	//pugi::xml_document data;
	//pugi::xml_node root;

	//root = data.append_child("game_state");

	//std::list<Module*>::iterator item = modules.begin();
	//ret = true;

	//for (item; item != modules.end() && ret == true; item = next(item))
	//{
	//	ret = (*item)->Save(root.append_child((*item)->name.c_str()));
	//}

	//if (ret == true)
	//{
	//	data.save_file(save_game_file.c_str());
	//	LOG("... finished saving", );
	//}
	//else
	//	LOG("Save process halted from an error in module %s", (*item != NULL) ? (*item)->name.c_str() : "unknown");

	//data.reset();
	return ret;
}