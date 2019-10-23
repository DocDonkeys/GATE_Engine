#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "GeometryLoader.h"
#include "libs/imgui/imgui_impl_sdl.h"

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
	delete[] keyboard;
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
	
	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer pre-Update", Profiler::Color::OrangeRed);

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

	mouse_x /= App->window->window_scale;
	mouse_y /= App->window->window_scale;
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
			mouse_x = e.motion.x / App->window->window_scale;
			mouse_y = e.motion.y / App->window->window_scale;

			mouse_x_motion = e.motion.xrel / App->window->window_scale;
			mouse_y_motion = e.motion.yrel / App->window->window_scale;
			break;

			//case SDL_DROPBEGIN:     // Drop Start
			//	quit = false;
			//	break;

			case SDL_DROPFILE:     // In case if dropped file	//CHANGE/FIX: Doesn't work consistently, sometimes the drop event is not called
				dropFileDir = e.drop.file;
				
				LOG("File dropped on window: %s", dropFileDir);

				SDL_free(dropFileDir);

				App->geometry_loader->Load3DFile(dropFileDir);
				LOG("File dropped on window: %s", dropFileDir);

				SDL_free(dropFileDir);

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

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}