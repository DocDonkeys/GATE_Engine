#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

// Memory Leak Detection
#include "MemLeaks.h"

ModuleWindow::ModuleWindow(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	int initRes = SDL_Init(SDL_INIT_VIDEO);
	if(initRes < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		SDL_assert(initRes >= 0);
		ret = false;
	}
	else
	{
		// Get screen data
		const HWND hDesktop = GetDesktopWindow();
		HMONITOR monitor = MonitorFromWindow(hDesktop, MONITOR_DEFAULTTONEAREST);
		MONITORINFO info;
		info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &info);
		monitor_width = info.rcMonitor.right - info.rcMonitor.left;
		monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

		//Create window
		int width = window_width * window_scale;
		int height = window_height * window_scale;
		int max_width = window_max_width * window_scale;
		int max_height = window_max_height * window_scale;

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		//
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // type of GL context (Core, Compatibility, ES). See SDL_GLprofile
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //whether the output is single or double buffered;
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); //the minimum number of bits in the depth buffer; defaults to 16
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); //the minimum number of bits in the stencil buffer; defaults to 0

		if(App->window->window_fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(App->window->window_resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(App->window->window_borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(App->window->window_full_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			SDL_assert(window != NULL);
			ret = false;
		}
		else
		{
			SDL_SetWindowMinimumSize(window, max_width, max_height);

			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}
	
	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

int ModuleWindow::GetWidth() const
{
	return window_width;
}

int ModuleWindow::GetHeight() const
{
	return window_height;
}

int ModuleWindow::GetScale() const
{
	return window_scale;
}

float ModuleWindow::GetBrightness() const
{
	return window_brightness;
}

int ModuleWindow::GetMonitorWidth() const
{
	return monitor_width;
}

int ModuleWindow::GetMonitorHeight() const
{
	return monitor_width;
}

bool ModuleWindow::GetFullscreen() const
{
	return window_fullscreen;
}

bool ModuleWindow::GetResizable() const
{
	return window_resizable;
}

bool ModuleWindow::GetBorderless() const
{
	return window_borderless;
}

bool ModuleWindow::GetFullDesktop() const
{
	return window_full_desktop;
}

void ModuleWindow::ResizeWindow(const uint& width, const uint& height)
{
	SDL_SetWindowSize(window,width,height);

	window_width = width;
	window_width = height;
}

void ModuleWindow::ChangeWindowBrightnessTo(float brightness)
{
	if (brightness < 0.000f)
		brightness = 0.000f;
	else if (brightness > 1.000f)
		brightness = 1.000f;

	SDL_SetWindowBrightness(window,brightness);

	window_brightness = brightness;
}

void ModuleWindow::WindowSetFullscreen(const bool & fullscreen)
{
	if (fullscreen)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowFullscreen(window, 0);

	window_fullscreen = fullscreen;
}

void ModuleWindow::WindowSetResizable(const bool & resizable)
{
	SDL_SetWindowResizable(window, (SDL_bool)resizable);

	window_resizable = resizable;
}

void ModuleWindow::WindowSetBorderless(const bool & borderless)
{
	SDL_SetWindowBordered(window,(SDL_bool)!borderless); // Passing true to the function enables borders, passing false sets borderless flag

	window_borderless = borderless;
}

void ModuleWindow::WindowSetFullscreenDesktop(const bool & fullscreen_desktop)
{
	if (fullscreen_desktop)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);

	window_full_desktop = fullscreen_desktop;
}
