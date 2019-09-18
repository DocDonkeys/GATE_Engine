#ifndef __MODULEENGINEGUI_H__
#define __MODULEENGINEGUI_H__


#include "Globals.h"
#include "Module.h"

class ModuleEngineGUI : public Module
{
public:
	// CONSTRUCTOR
	ModuleEngineGUI(Application* app, bool start_enabled = true);
	//DESTRUCTOR
	~ModuleEngineGUI();

public:
	bool Init();
	bool Start();
	update_status Update(float dt);

};

#endif // !__MODULEENGINEGUI_H__

