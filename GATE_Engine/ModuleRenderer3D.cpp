#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "Mesh.h"

#include "libs/glew/include/GL/glew.h"
#include "libs/SDL/include/SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

ModuleRenderer3D::ModuleRenderer3D(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_assert(context != NULL);
		ret = false;
	}

	if(ret == true)
	{
		//Use Vsync
		if (App->renderer3D->vSync) {
			int swapInt = SDL_GL_SetSwapInterval(1);
			if (swapInt < 0) {
				LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				SDL_assert(swapInt >= 0);
			}
		}

		//Init OpenGL wth Glew
		GLenum err = glewInit();

		if (err != GLEW_OK) // glewInit error
		{
			LOG("Could not Initialize Glew, %s", glewGetErrorString(err));
			return false;
		}
		else // Success!
		{
			LOG("Succesfully initialized Glew!");
			LOG("Using Glew %s", glewGetString(GLEW_VERSION));

			//LOG Hardware
			LOG("Vendor: %s", glGetString(GL_VENDOR));
			LOG("Renderer: %s", glGetString(GL_RENDERER));
			LOG("OpenGL version supported %s", glGetString(GL_VERSION));
			LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		}


		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error	//IMPROVE: This error check code is repeated several times, make a function
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			SDL_assert(error == GL_NO_ERROR);
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			SDL_assert(error == GL_NO_ERROR);
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			SDL_assert(error == GL_NO_ERROR);
			ret = false;
		}

		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		lights[0].Active(true);

		//GL_Settings Startup
		GLSettingsSetup();
	}

	// Projection matrix for
	OnResize(App->window->window_width, App->window->window_height);

	return ret;
}

bool ModuleRenderer3D::Start()
{
	bool ret = true;

	App->texture_loader->LoadTextureFile("Assets\\Textures\\Lenna_(test_image).png");	//CHANGE/FIX: Hardcoded, either put it on a JSON file to load as "default textures" or just erase it
	App->texture_loader->LoadTextureFile("Assets\\Textures\\Baker_house.png");
	App->texture_loader->LoadDefaultTex();
	
	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer pre-Update", Profiler::Color::Orange);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer post-Update", Profiler::Color::DarkOrange);

	//DIRECT MODE Rendering
	glLineWidth(2.0f);

	glBegin(GL_LINES);
	//PLANE
	for (float i = -50; i <= 50; ++i)
	{
		glVertex3f(i, 0.f, -50.f);
		glVertex3f(i, 0, 50.f);

		glVertex3f(-50.f, 0.f, i);
		glVertex3f(50.f, 0, i);
	}
	glEnd();

	glLineWidth(1.0f);
	//Render

	//Debug Draw (Render)

	//Render GUI
	App->editor->RenderEditorUI();

	//Swap Window
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// GL Settings
void ModuleRenderer3D::GLSettingsSetup()
{
	SetGLSetting(GL_DepthTest);
	SetGLSetting(GL_CullFace);
	SetGLSetting(GL_Lighting);
	SetGLSetting(GL_ColorMaterial);
	SetGLSetting(GL_Texture2D);
	SetGLSetting(GL_TextureCubeMap);
	SetGLSetting(GL_Blend);
	SetGLSetting(GL_Dither);
	SetGLSetting(GL_PointSmooth);
	SetGLSetting(GL_LineSmooth);
	SetGLSetting(GL_LineStipple);
	SetGLSetting(GL_PolygonSmooth);
	SetGLSetting(GL_PolygonStipple);
	SetGLSetting(GL_MinMax);
	SetGLSetting(GL_MultiSample);
}

void ModuleRenderer3D::SetGLSetting(GL_Setting& glSet) const	// Enables GL setting if bool flag marks so
{
	if (glSet.status) {
		glEnable(glSet.id);
	}
}

void ModuleRenderer3D::SwitchGLSetting(GL_Setting& glSet) const	// Switches setting value from on/off or viceversa
{
	if (glIsEnabled(glSet.id)) {
		glDisable(glSet.id);
		glSet.status = false;
	}
	else {
		glEnable(glSet.id);
		glSet.status = true;
	}
}


void ModuleRenderer3D::SwitchGroupGLSetting(GL_Setting& glSet, std::vector<GL_Setting*>* group) const
{
	if (!glIsEnabled(glSet.id)) {

		if (group != nullptr) {
			for (std::vector<GL_Setting*>::iterator it = group->begin(); it != group->end(); it = next(it)) {
				if ((*it)->status) {
					glDisable((*it)->id);
					(*it)->status = false;
					break;
				}
			}
		}

		glEnable(glSet.id);
		glSet.status = true;
	}
}

void ModuleRenderer3D::GenerateVertexBuffer(uint & id_vertex, const int& size, const float3 * vertex)
{
	glGenBuffers(1, (GLuint*) &(id_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * size, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRenderer3D::GenerateVertexBuffer(uint & id_vertex, const int & size, const float * vertex)
{
	glGenBuffers(1, (GLuint*) &(id_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRenderer3D::GenerateIndexBuffer(uint & id_index,const int& size, const uint * index)
{
	glGenBuffers(1, (GLuint*) &(id_index));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * size, index, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRenderer3D::DeleteBuffer(uint & id)
{
	glDeleteBuffers(1,&(GLuint)id);
	id = 0;
}

//Pass a Mesh_Data to be drawn using glDrawElements
void ModuleRenderer3D::DrawMesh(const Mesh* mesh, const uint texture_id)
{
	//Draw VERTICES with INDICES
	if (mesh->index != nullptr) //We need indices to use DrawElements if we don't have any we would crash openGL
	{
		if (texture_id != 0) // The mesh has a texture to be printed
			PrintTexturedMesh(mesh, texture_id);
		else
			PrintSimpleMesh(mesh);
	}
	else
		LOG("WARNING! Tried to draw mesh with id_vertex: %d using DrawElements, but the mesh doesn't contain indices!");
}

//DRAW a Mesh with only vertices and indices data
void ModuleRenderer3D::PrintSimpleMesh(const Mesh* mesh)
{
	glEnableClientState(GL_VERTEX_ARRAY);

	//Bind buffers
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);

	//Draw
	glDrawElements(GL_TRIANGLES, mesh->num_index, GL_UNSIGNED_INT, nullptr);

	//Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
}

//DRAW a mesh that contains textures using it's texture coordinates
void ModuleRenderer3D::PrintTexturedMesh(const Mesh * mesh, const uint texture_id)
{
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Bind Buffers
	glBindTexture(GL_TEXTURE_2D, texture_id); // Texture
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_tex_coords); // Texture Coordinates
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	//Draw
	PrintSimpleMesh(mesh); // Once we have binded the necessary stuff for textured rendering, we render

	//Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ModuleRenderer3D::DrawMeshFaceNormals(Mesh * mesh)
{
	//Print faces normals
	if (drawFaceNormals) {
		glBegin(GL_LINES);
		glColor3f(1, 0, 1);

		for (int i = 0; i < mesh->num_index; ++i)
		{
			glVertex3f(mesh->normals_faces[i].x, mesh->normals_faces[i].y, mesh->normals_faces[i].z);
			glVertex3f(mesh->normals_faces[i].x + mesh->normals_faces_vector[i].x,
				mesh->normals_faces[i].y + mesh->normals_faces_vector[i].y,
				mesh->normals_faces[i].z + mesh->normals_faces_vector[i].z);
		}
		glColor3f(1, 1, 1);
		glEnd();
	}
}

void ModuleRenderer3D::DrawMeshVertexNormals(Mesh * mesh)
{
	//Draw Vertex Normals
	if (drawVertexNormals && mesh->normals_vector != nullptr)
	{
		//Had a lot of problems with buffers so we draw in direct mode, we can save some VRAM space
		glBegin(GL_LINES);
		glColor3f(0, 1, 0);

		for (int i = 0; i < mesh->num_vertex; ++i)
		{
			glVertex3f(mesh->vertex[i].x, mesh->vertex[i].y, mesh->vertex[i].z);
			glVertex3f(mesh->vertex[i].x + mesh->normals_vector[i].x,
				mesh->vertex[i].y + mesh->normals_vector[i].y,
				mesh->vertex[i].z + mesh->normals_vector[i].z);
		}
		glColor3f(1, 1, 1);
		glEnd();
	}
}
