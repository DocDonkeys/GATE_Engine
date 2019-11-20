#ifndef __MODULERENDERER_H__
#define __MODULERENDERER_H__

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "libs/MathGeoLib/include/Math/float3x3.h"
#include "libs/MathGeoLib/include/Math/float4x4.h"

#define MAX_LIGHTS 8

class Mesh;

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

	//Tools for other modules
	void GenerateVertexBuffer(uint& id_vertex, const int& size, const float3* vertex);
	void GenerateVertexBuffer(uint& id_vertex, const int& size, const float* vertex);
	void GenerateIndexBuffer(uint& id_index, const int& size, const uint* index);
	void DeleteBuffer(uint& id);
	//Draw a given mesh
	void DrawMesh(const Mesh* mesh, const uint texture_id = 0);
	//Print the Mesh with vertices and indices only
	void PrintSimpleMesh(const Mesh* mesh);
	//DRAW a mesh that contains textures using its texture coordinates
	void PrintTexturedMesh(const Mesh* mesh, const uint texture_id);

	//Draw Mesh Debug Normals
	void DrawMeshFaceNormals(Mesh* mesh);
	void DrawMeshVertexNormals(Mesh* mesh);

	// OpenGL Settings
	struct GL_Setting {
		uint id;
		std::vector<GL_Setting*>* group;	//Used for grouping settings that are mutually exclusive
		bool status;
	};
	void SwitchGLSetting(GL_Setting& glSet) const;
	void SwitchGroupGLSetting(GL_Setting& glSet, std::vector<GL_Setting*>* group) const;

private:
	void GLSettingsSetup();
	void SetGLSetting(GL_Setting& glSet) const;

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	float3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool vSync = true;
	bool drawVertexNormals = false;	//CHANGE/FIX: Save&Load
	bool drawFaceNormals = false;	//CHANGE/FIX: Save&Load
	bool drawObjAABB = false;		//CHANGE/FIX: Save&Load
	bool drawObjOBB = false;		//CHANGE/FIX: Save&Load
	bool drawStaticTree = false;	//CHANGE/FIX: Save&Load
	bool drawGrid = true;			//CHANGE/FIX: Save&Load

	//OpenGL Flags https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glEnable.xml	//IMPROVE: Make a list jesus
	GL_Setting GL_DepthTest;		//Do depth comparisons and update the depth buffer.
	GL_Setting GL_CullFace;			//Cull polygons based on their winding in window coordinates.
	GL_Setting GL_Lighting;			//If no vertex shader is active, use the current lighting parameters to compute the vertex color or index.
	
	std::vector<GL_Setting*> GL_TextureModes;
	GL_Setting GL_Texture2D;		//If no fragment shader is active, two - dimensional texturing is performed
	GL_Setting GL_TextureCubeMap;	//If no fragment shader is active, cube - mapped texturing is performed.
	
	GL_Setting GL_PointSmooth;		//If enabled, draw points with proper filtering. Otherwise, draw aliased points
	GL_Setting GL_LineSmooth;		//If enabled draw lines with correct filtering. Otherwise, draw aliased lines.
	GL_Setting GL_PolygonSmooth;	//If enabled, draw polygons with proper filtering. Otherwise, draw aliased polygons. (For correct antialiased polygons, an alpha buffer is needed and the polygons must be sorted front to back.)
	
	GL_Setting GL_LineStipple;		//Use the current line stipple pattern when drawing lines.
	GL_Setting GL_PolygonStipple;	//Use the current polygon stipple pattern when rendering polygons.

	GL_Setting GL_Blend;			//Blend the computed fragment color values with the values in the color buffers.
	GL_Setting GL_Dither;			//Dither color components or indices before they are written to the color buffer.

	GL_Setting GL_ColorMaterial;	//Have one or more material parameters track the current color.
	GL_Setting GL_MinMax;			//Compute the minimum and maximum values of incoming RGBA color values.
	GL_Setting GL_MultiSample;		//Use multiple fragment samples in computing the final color of a pixel.
};


#endif