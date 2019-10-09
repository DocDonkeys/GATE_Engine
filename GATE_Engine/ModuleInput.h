#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>

#define MAX_MOUSE_BUTTONS 5

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleInput();

	bool Init();
	update_status PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	// Gather relevant win events
	bool GetWindowEvent(EventWindow ev) {
		return windowEvents[ev];
	}

public:
	std::vector<unsigned char> input_log;
	std::vector<unsigned char*> input_type_log;

	char* dropFileDir;

private:
	bool windowEvents[WE_COUNT];

	//Button mapping
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];

	//Mouse position and movement
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	//int mouse_z_motion;

	unsigned char key_down[5] = "DOWN";
	unsigned char key_repeat[7] = "REPEAT";
	unsigned char key_up[3] = "UP";
};