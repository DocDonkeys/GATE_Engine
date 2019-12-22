#ifndef __MODULESCRIPTING_H__
#define __MODULESCRIPTING_H__

#include "Module.h"

class ModuleScripting : public Module
{
public:
	ModuleScripting(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleScripting();

public:

	bool Init();
	bool CleanUp();
	update_status Update(float dt);

};



#endif // !__MODULESCRIPTING_H__

