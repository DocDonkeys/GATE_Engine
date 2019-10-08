#include "GeometryLoader.h"
#include "Application.h"


#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cfileio.h>

#pragma comment (lib, "libs/assimp-5.0.0/libx86/assimp.lib")

//#pragma comment (lib, "libs/assimp-5.0.0/libx86/assimp.lib")
/*
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h" 
#include "Assimp/include/postprocess.h" 
#include "Assimp/include/cfileio.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")
*/

GeometryLoader::GeometryLoader(Application * app, const char * name, bool start_enabled) : Module(app, name, start_enabled)
{
}

GeometryLoader::~GeometryLoader()
{
}

bool GeometryLoader::CleanUp()
{
	bool ret = true;

	// detach log stream 
	aiDetachAllLogStreams();

	return ret;
}

bool GeometryLoader::Init()
{
	// Stream log messages to Debug window 
	struct aiLogStream stream; 
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr); 
	aiAttachLogStream(&stream);

	return true;
}

bool GeometryLoader::Start()
{
	return true;
}

update_status GeometryLoader::Update(float dt)
{
	return UPDATE_CONTINUE;
}
