#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "libs/glew/include/GL/glew.h"
#include "libs/SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	App->ConsoleLOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		App->ConsoleLOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_TriggerBreakpoint();
		ret = false;
	}

	if(ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0) {
			App->ConsoleLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			SDL_TriggerBreakpoint();
		}

		//Init OpenGL wth Glew
		GLenum err = glewInit();

		if (err != GLEW_OK) // glewInit error
		{
			App->ConsoleLOG("Could not Initialize Glew, %s", glewGetErrorString(err));
			return false;
		}
		else // Success! 
		{
			App->ConsoleLOG("Succesfully initialized Glew!");
			App->ConsoleLOG("Using Glew %s", glewGetString(GLEW_VERSION));

			//LOG Hardware
			App->ConsoleLOG("Vendor: %s", glGetString(GL_VENDOR)); 
			App->ConsoleLOG("Renderer: %s", glGetString(GL_RENDERER));
			App->ConsoleLOG("OpenGL version supported %s", glGetString(GL_VERSION));
			App->ConsoleLOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		}
			

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			App->ConsoleLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			SDL_TriggerBreakpoint();
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			App->ConsoleLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			SDL_TriggerBreakpoint();
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
			App->ConsoleLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			SDL_TriggerBreakpoint();
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
		
		glEnable(GL_DEPTH_TEST); //ASK Marc
		glEnable(GL_CULL_FACE); //ASK Marc
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D); // ASK Marc
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

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

	//Change some options GL_DEPTH_TEST,GL_CULL_FACE,GL_LIGHTING ○ GL_COLOR_MATERIAL,GL_TEXTURE_2D + two other
	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		if (glIsEnabled(GL_DEPTH_TEST))
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
	}
		

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
	{
		if (glIsEnabled(GL_CULL_FACE))
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);
	}

	if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
	{
		if (glIsEnabled(GL_LIGHTING))
			glDisable(GL_LIGHTING);
		else
			glEnable(GL_LIGHTING);
	}

	if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
	{
		if (glIsEnabled(GL_COLOR_MATERIAL))
			glDisable(GL_COLOR_MATERIAL);
		else
			glEnable(GL_COLOR_MATERIAL);
	}

	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
	{
		if (glIsEnabled(GL_TEXTURE_2D))
			glDisable(GL_TEXTURE_2D);
		else
			glEnable(GL_TEXTURE_2D);
	}

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Renderer post-Update", Profiler::Color::DarkOrange);

	//TEST
	glLineWidth(2.0f);

	glBegin(GL_LINES);
	//PLANE
	for (float i = 0; i < 10; ++i)
	{
		glVertex3f(i, 0.f, 0.f);
		glVertex3f(i, 0, 10.f);

		glVertex3f(0.f, 0.f, i);
		glVertex3f(10.f, 0, i);
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	float size = 1;

	//1st face
	glVertex3f(0, size, 0.f);
	glVertex3f(0, 0.f, 0.f);
	glVertex3f(0, 0.f, size);

	glVertex3f(0, size, 0.f);
	glVertex3f(0, 0.f, size);
	glVertex3f(0, size, size);

	//2nd face
	glVertex3f(0.f,size,size);
	glVertex3f(0.f,0.f,size);
	glVertex3f(size,0.f,size);

	glVertex3f(0.f,size,size);
	glVertex3f(size,0.f,size);
	glVertex3f(size, size, size);

	//3rd face
	glVertex3f(0, size, 0.f);
	glVertex3f(0, size, size);
	glVertex3f(size, size, size);

	glVertex3f(0, size, 0.f);
	glVertex3f(size, size, size);
	glVertex3f(size, size, 0);

	//4th 5th & 6th face are 1st, 2nd & 3rd in reverse order to change the direction of the normal, and then we displace it by size in the direction we need

	//4th face
	glVertex3f(size, 0.f, size);
	glVertex3f(size, 0.f, 0.f);
	glVertex3f(size, size, 0.f);
	
	glVertex3f(size, size, size);
	glVertex3f(size, 0.f, size);
	glVertex3f(size, size, 0.f);
	
	//For now the size of the buffer is the size of the cube
	int cube_buffer_size = 6 * 6;

	uint my_id = 0; 
	glGenBuffers(1, (GLuint*) &(my_id)); 
	glBindBuffer(GL_ARRAY_BUFFER, my_id); 
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)* cube_buffer_size * 3, vertices, GL_STATIC_DRAW);
	

	glEnd();

	glLineWidth(1.0f);
	//Render

	//Debug Draw (Render)

	//Render GUI
	App->engineGUI->RenderGUI();

	//Swap Window
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	App->ConsoleLOG("Destroying 3D Renderer");

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
