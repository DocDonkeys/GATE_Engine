#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

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

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		SDL_TriggerBreakpoint();
		ret = false;
	}
	else
	{
		//Create window
		int width = App->engineGUI->GetWinWidth() * App->engineGUI->GetWinScale();
		int height = App->engineGUI->GetWinHeight() * App->engineGUI->GetWinScale();
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		//
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // type of GL context (Core, Compatibility, ES). See SDL_GLprofile
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); //whether the output is single or double buffered;
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); //the minimum number of bits in the depth buffer; defaults to 16
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); //the minimum number of bits in the stencil buffer; defaults to 0

		if(App->engineGUI->IsFullscreen() == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(App->engineGUI->IsResizable() == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(App->engineGUI->IsBorderless() == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(App->engineGUI->IsFullDesktop() == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			SDL_TriggerBreakpoint();
			ret = false;
		}
		else
		{
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

void ModuleWindow::ResizeWindow(const uint& width, const uint& height)
{
	SDL_SetWindowSize(window,width,height);
}

void ModuleWindow::ChangeWindowBrightnessTo(float brightness)
{
	if (brightness < 0.000f)
		brightness = 0.000f;
	else if (brightness > 1.000f)
		brightness = 1.000f;

	SDL_SetWindowBrightness(window,brightness);
}

void ModuleWindow::WindowSetFullscreen(const bool & fullscreen)
{
	if (fullscreen)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::WindowSetResizable(const bool & resizable)
{
	SDL_SetWindowResizable(window, (SDL_bool)resizable);
}

void ModuleWindow::WindowSetBorderless(const bool & borderless)
{
	SDL_SetWindowBordered(window,(SDL_bool)!borderless); // Passing true to the function enables borders, passing false sets borderless flag
}

void ModuleWindow::WindowSetFullscreenDesktop(const bool & fullscreen_desktop)
{
	if (fullscreen_desktop)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);
}
