#include "ModuleWindow.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleSceneIntro.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"
#include "GeometryLoader.h"
#include "libs/imgui/imgui_impl_sdl.h"

#include <Windows.h>

// Brofiler
#include "libs/Brofiler/Brofiler.h"

// Memory Leak Detection
#include "MemLeaks.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	int initSysRes = SDL_InitSubSystem(SDL_INIT_EVENTS);
	if (initSysRes < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		SDL_assert(initSysRes >= 0);
		ret = false;
	}

	extension_3D_file.push_back("fbx");
	extension_3D_file.push_back("FBX");
	extension_3D_file.push_back("obj");
	extension_3D_file.push_back("OBJ");

	extension_texture.push_back("png");
	extension_texture.push_back("PNG");
	extension_texture.push_back("jpg");
	extension_texture.push_back("dds");
	extension_texture.push_back("DDS");
	extension_texture.push_back("tga");

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer pre-Update", Profiler::Color::OrangeRed);

	// Screen Wrapping
	if (GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {

		// Monitor x goes from 0 to width_x - 1
		POINT p;
		if (GetCursorPos(&p))
		{
			int width = App->window->GetMonitorWidth();

			if (p.x > width - 5) {
				SetCursorPos(10, p.y);
				wrappedMouse = true;
			}
			else if (p.x < 5) {
				SetCursorPos(width - 10, p.y);
				wrappedMouse = true;
			}
		}
	}

	// Events
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
			{
				keyboard[i] = KEY_DOWN;
				input_log.push_back(i);
				input_type_log.push_back(key_down);
			}
			else
			{
				keyboard[i] = KEY_REPEAT;
				if (input_type_log.back() != key_repeat)
				{
					input_log.push_back(i);
					input_type_log.push_back(key_repeat);
				}
			}
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
			{
				keyboard[i] = KEY_UP;
				input_log.push_back(i);
				input_type_log.push_back(key_up);
			}
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= App->window->GetScale();
	mouse_y /= App->window->GetScale();
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	//This will be used if a file is dragged & dropped into the engine, we will determine which function we call with it
	std::string file_extension;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
				if (!wrappedMouse) {
					mouse_x = e.motion.x / App->window->GetScale();
					mouse_y = e.motion.y / App->window->GetScale();

					mouse_x_motion = e.motion.xrel / App->window->GetScale();
					mouse_y_motion = e.motion.yrel / App->window->GetScale();
				}
			break;

			//case SDL_DROPBEGIN:     // Drop Start
			//	quit = false;
			//	break;

			case SDL_DROPFILE:     // In case if dropped file	//CHANGE/FIX: Doesn't work consistently, sometimes the drop event is not called
				dropFileDir = e.drop.file;

				LOG("File dropped on window: %s", dropFileDir);

				SDL_free(dropFileDir);	//CHANGE/FIX: Should this be here?

				App->resources->ImportFile(dropFileDir);

				//Chop path to get file extension
				/*file_extension = dropFileDir;
				file_extension = App->SubtractString(file_extension, ".", true, false);*/

				//TODO: Didac Module resources will manage this decision, keeping here for reference
				//for (int i = 0; i < extension_3D_file.size(); ++i) { //Check if the extension is a Mesh or 3D object scene
				//	if (extension_3D_file[i] == file_extension) {
				//		App->geometry_loader->Load3DFile(dropFileDir);
				//		break;
				//	}
				//}

				//for (int i = 0; i < extension_texture.size(); ++i)	//Check if the extension is a texture
				//{
				//	if (extension_texture[i] == file_extension)
				//	{
				//		if (App->scene_intro->selected_go != nullptr)
				//			App->scene_intro->AddTextureToGameObject(App->scene_intro->selected_go, dropFileDir);
				//		else
				//			LOG("Tried to drag & drop a texture, but no Game Object is selected! Please select a Game Object.");
				//			break;
				//	}
				//}

				/*CHANGE/FIX:
				1. DO OPEN FILE OPERATION OR FLAG ITS START
				2. LOG("File dropped on window: %s", dropFileDir);
				3. SDL_free(dropFileDir);    // Free dropped_filedir memory
				*/
				break;

			//case SDL_DROPCOMPLETE:     // Drop end
			//	quit = false;
			//	break;

			case SDL_QUIT:
				windowEvents[WE_QUIT] = true;
				break;

			case SDL_WINDOWEVENT:
				switch (e.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
				case SDL_WINDOWEVENT_HIDDEN:
				case SDL_WINDOWEVENT_MINIMIZED:
				case SDL_WINDOWEVENT_RESIZED:
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					windowEvents[WE_HIDE] = true;
					break;

					//case SDL_WINDOWEVENT_ENTER:
				case SDL_WINDOWEVENT_SHOWN:
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				case SDL_WINDOWEVENT_MAXIMIZED:
				case SDL_WINDOWEVENT_RESTORED:
					windowEvents[WE_SHOW] = true;
					break;
				}
				break;
		}

		//Pass the event to IMGUI for input and scrolling purposes
		ImGui_ImplSDL2_ProcessEvent(&e);
	}

	return UPDATE_CONTINUE;
}

// After all updates are done, mark mouse wrapping as over
update_status ModuleInput::PostUpdate(float dt)
{
	wrappedMouse = false;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
