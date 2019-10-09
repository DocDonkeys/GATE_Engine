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
	void SetGLSetting(GLenum id, bool status) const;
	bool SwitchGLSetting(GLenum id) const;

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool vSync = true;

	//OpenGL Flags https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glEnable.xml
	bool GL_DepthTest;		//Do depth comparisons and update the depth buffer.
	bool GL_CullFace;		//Cull polygons based on their winding in window coordinates.
	bool GL_Lighting;		//If no vertex shader is active, use the current lighting parameters to compute the vertex color or index.
	
	bool GL_Texture2D;		//If no fragment shader is active, two - dimensional texturing is performed
	bool GL_TextureCubeMap;	//If no fragment shader is active, cube - mapped texturing is performed.
	
	bool GL_PointSmooth;	//If enabled, draw points with proper filtering. Otherwise, draw aliased points
	bool GL_LineSmooth;		//If enabled draw lines with correct filtering. Otherwise, draw aliased lines.
	bool GL_PolygonSmooth;	//If enabled, draw polygons with proper filtering. Otherwise, draw aliased polygons. (For correct antialiased polygons, an alpha buffer is needed and the polygons must be sorted front to back.)
	
	bool GL_LineStipple;	//Use the current line stipple pattern when drawing lines.
	bool GL_PolygonStipple;	//Use the current polygon stipple pattern when rendering polygons.

	bool GL_Blend;			//Blend the computed fragment color values with the values in the color buffers.
	bool GL_Dither;			//Dither color components or indices before they are written to the color buffer.

	bool GL_ColorMaterial;	//Have one or more material parameters track the current color.
	bool GL_MinMax;			//Compute the minimum and maximum values of incoming RGBA color values.
	bool GL_MultiSample;	//Use multiple fragment samples in computing the final color of a pixel.

	////////////////
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
