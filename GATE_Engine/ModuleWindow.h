#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "libs/SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
	friend class Application;	//CHANGE/FIX: This shouldn't be here. This is for the Save&Load on Application, window and every other module should have a Save&Load to exec on it's own.
	friend class ModuleRenderer3D;
	friend class ModuleEditor;

public:
	ModuleWindow(Application* app, const char* name = "null", bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

public:
	void SetTitle(const char* title);

	int GetWidth() const;
	int GetHeight() const;
	int GetScale() const;
	float GetBrightness() const;

	int GetMonitorWidth() const;
	int GetMonitorHeight() const;

	bool GetFullscreen() const;
	bool GetResizable() const;
	bool GetBorderless() const;
	bool GetFullDesktop() const;

	void ResizeWindow(const uint& width, const uint& height);
	void ChangeWindowBrightnessTo(float brightness);
	void WindowSetFullscreen(const bool& fullscreen);
	void WindowSetResizable(const bool& resizable);
	void WindowSetBorderless(const bool& borderless);
	void WindowSetFullscreenDesktop(const bool& fullscreen_desktop);

private:
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