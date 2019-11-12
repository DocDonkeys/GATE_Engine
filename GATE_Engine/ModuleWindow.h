#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "libs/SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, const char* name = "null", bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);

public:
	void ResizeWindow(const uint& width, const uint& height);
	void ChangeWindowBrightnessTo(float brightness);
	void WindowSetFullscreen(const bool& fullscreen);
	void WindowSetResizable(const bool& resizable);
	void WindowSetBorderless(const bool& borderless);
	void WindowSetFullscreenDesktop(const bool& fullscreen_desktop);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	// Monitor data
	int monitor_width;
	int monitor_height;

	//Window data
	int window_width = 1280;	//CHANGE/FIX: Save/Load
	int window_height = 1024;
	int window_scale = 1;
	float window_brightness = 1.0f;

	//Window flags
	bool window_fullscreen = false;
	bool window_resizable = true;
	bool window_borderless = false;
	bool window_full_desktop = false;
};

#endif // __ModuleWindow_H__