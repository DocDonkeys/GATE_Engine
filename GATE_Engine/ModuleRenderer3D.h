#ifndef __MODULERENDERER_H__
#define __MODULERENDERER_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "Primitive.h"

#define MAX_LIGHTS 8

class Primitive;

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

	// OpenGL Settings
	bool SetGLSetting(GLenum id, bool status) const;
	bool SwitchGLSetting(GLenum id) const;

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool vSync = true;

	//OpenGL Flags https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glEnable.xml
	bool GL_DepthTest;
	bool GL_CullFace;
	bool GL_Lightning;
	bool GL_ColorMaterial;
	bool GL_Texture2D;
	//CHANGE/FIX: Add more toggle options

	uint vertex_array_id = 0; // vertex array ID for example 1 of drawing with vertex buffers
	uint vertex_optimized_array_id = 0;
	uint indices_id = 0;

	//JUST TO SHOW THE SQUARES ROTATE, MUST BE REMOVED WHEN NO LONGER REQUIRED
	float x_rot, y_rot, z_rot = 1.f;
	float angle_rot = 0.0f;

	//TEST
	Primitive* sphere_test = nullptr;
};

#endif
