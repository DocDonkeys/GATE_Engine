#ifndef __GEOMETRYLOADER_H__
#define __GEOMETRYLOADER_H__

#include "Globals.h"
#include "Module.h"

class GeometryLoader : public Module
{
public:
	GeometryLoader(Application* app, const char* name = "null", bool start_enabled = true);
	~GeometryLoader();

public:
	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();



public: // Vars
};


#endif //__GEOMETRYLOADER_H__