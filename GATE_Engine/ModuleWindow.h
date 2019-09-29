#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "libs/SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

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
};

#endif // __ModuleWindow_H__