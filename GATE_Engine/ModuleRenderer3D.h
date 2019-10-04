#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, const char* name = "null", bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

public:
	void OnResize(int width, int height);

	bool IsVSynced() const;

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private:
	bool vSync = true;	//TODO: Save/Load

	uint vertex_array_id = 0; // vertex array ID for example 1 of drawing with vertex buffers
	uint vertex_optimized_array_id = 0;
	uint indices_id = 0;

};