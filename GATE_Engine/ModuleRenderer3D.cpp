#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"


#include "libs/glew/include/GL/glew.h"
#include "libs/SDL/include/SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
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
		if (App->renderer3D->vSync) {
			if (SDL_GL_SetSwapInterval(1) < 0) {
				App->ConsoleLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				SDL_TriggerBreakpoint();
			}
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
		
		lights[0].Active(true);

		//GL_Settings Startup
		SetGLSetting(GL_DEPTH_TEST, GL_DepthTest);
		SetGLSetting(GL_CULL_FACE, GL_CullFace);
		SetGLSetting(GL_LIGHTING, GL_Lighting);
		SetGLSetting(GL_COLOR_MATERIAL, GL_ColorMaterial);
		SetGLSetting(GL_TEXTURE_2D, GL_Texture2D);
		SetGLSetting(GL_TEXTURE_CUBE_MAP, GL_TextureCubeMap);
		SetGLSetting(GL_BLEND, GL_Blend);
		SetGLSetting(GL_DITHER, GL_Dither);
		SetGLSetting(GL_POINT_SMOOTH, GL_PointSmooth);
		SetGLSetting(GL_LINE_SMOOTH, GL_LineSmooth);
		SetGLSetting(GL_LINE_STIPPLE, GL_LineStipple);
		SetGLSetting(GL_POLYGON_SMOOTH, GL_PolygonSmooth);
		SetGLSetting(GL_POLYGON_STIPPLE, GL_PolygonStipple);
		SetGLSetting(GL_MINMAX, GL_MinMax);
		SetGLSetting(GL_MULTISAMPLE, GL_MultiSample);
	}

	// Projection matrix for
	OnResize(App->window->window_width, App->window->window_height);

	return ret;
}

bool ModuleRenderer3D::Start()
{
	bool ret = true;

	//This code must be reestructured after next week class 08/10/19
	float size = 1.0f;

	float vertex_array_wduplication[108] = { //Vertex array for 1st example of frame buffers, vertices duplicated
	3.f, 1.f, 3.f,
	3.f, 0.f, 3.f,
	3.f, 0.f, 4.f,
	3.f, 1.f, 3.f,
	3.f, 0.f, 4.f,
	3.f, 1.f, 4.f,
	3.f, 1.f, 4.f,
	3.f, 0.f, 4.f,
	4.f, 0.f, 4.f,
	3.f, 1.f, 4.f,
	4.f, 0.f, 4.f,
	4.f, 1.f, 4.f,
	3.f, 1.f, 3.f,
	3.f, 1.f, 4.f,
	4.f, 1.f, 4.f,
	3.f, 1.f, 3.f,
	4.f, 1.f, 4.f,
	4.f, 1.f, 3.f,
	4.f, 0.f, 4.f,
	4.f, 0.f, 3.f,
	4.f, 1.f, 3.f,
	4.f, 1.f, 4.f,
	4.f, 0.f, 4.f,
	4.f, 1.f, 3.f,
	4.f, 0.f, 3.f,
	3.f, 0.f, 3.f,
	3.f, 1.f, 3.f,
	4.f, 1.f, 3.f,
	4.f, 0.f, 3.f,
	3.f, 1.f, 3.f,
	4.f, 0.f, 4.f,
	3.f, 0.f, 4.f,
	3.f, 0.f, 3.f,
	4.f, 0.f, 3.f,
	4.f, 0.f, 4.f,
	3.f, 0.f, 3.f};

	float vertex_array_findexing[24] = {
		0.f,1.f,0.f,
		0.f,0.f,0.f,
		0.f,0.f,1.f,
		0.f, 1.f, 1.f,
		1.f,1.f,0.f,
		1.f,0.f,0.f,
		1.f,0.f,1.f,
		1.f, 1.f, 1.f,
	};

	GLubyte indices[] = { 
		0,1,2, 2,3,0,   
		0,3,4, 3,7,4,
		3,2,7, 2,6,7,
		6,4,7, 4,6,5,
		1,0,4, 5,1,4,
		1,5,2, 2,5,6,

		/*0,3,4, 4,5,0,
		0,5,6, 6,1,0,
		1,6,7, 7,2,1,
		7,4,3, 3,2,7,
		4,7,6, 6,5,4*/ };

	//Prepare Buffers to be sent to VRAM
	glGenBuffers(1, (GLuint*) &(vertex_array_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_array_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, vertex_array_wduplication, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*) &(vertex_optimized_array_id));
	glBindBuffer(GL_ARRAY_BUFFER, vertex_optimized_array_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertex_array_findexing, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*) &(indices_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 36, indices, GL_STATIC_DRAW);

	sphere_test = new Primitive();
	sphere_test->CreateSphere(10,10);
	sphere_test->SendToVRAM();

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

	sphere_test->Draw();

	//App->scene_intro->sphere->draw(0, 5, 0);
	
	angle_rot += 30 *dt;
	glRotatef(angle_rot, 0, 1, 0);
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

	//4th 5th & 6th face are 1st, 2nd & 3rd in reverse order to change the direction of the normal, 
	// and then we displace it by size in the direction we need

	//4th face
	glVertex3f(size, 0.f, size);
	glVertex3f(size, 0.f, 0.f);
	glVertex3f(size, size, 0.f);
	
	glVertex3f(size, size, size);
	glVertex3f(size, 0.f, size);
	glVertex3f(size, size, 0.f);

	//5th face
	glVertex3f(size, 0.f, 0);
	glVertex3f(0.f, 0.f, 0);
	glVertex3f(0.f, size, 0);

	glVertex3f(size, size, 0);
	glVertex3f(size, 0.f, 0);
	glVertex3f(0.f, size, 0);

	//6th face
	glVertex3f(size, 0, size);
	glVertex3f(0, 0, size);
	glVertex3f(0, 0, 0.f);

	glVertex3f(size, 0, 0);
	glVertex3f(size, 0, size);
	glVertex3f(0, 0, 0.f);

	glEnd();

	glLineWidth(1.0f);

	//END of DIRECT MODE rendering

	glRotatef(angle_rot, 0, 1, 0);
	//START FRAME BUFFER Rendering
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_array_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableClientState(GL_VERTEX_ARRAY);

	//END FRAME BUFFER Rendering

	glRotatef(angle_rot, 0, 1, 0);
	glTranslatef(5.f,0.f,0.f);
	//START VERTICES & INDICES BUFFER Rendering
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertex_optimized_array_id);
	
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//glVertexPointer(1, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
	

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	
	

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

	RELEASE(sphere_test);

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
void ModuleRenderer3D::SetGLSetting(GLenum id, bool status) const	// Enables GL setting if bool flag marks so
{
	if (status) {
		glEnable(id);
	}
}

bool ModuleRenderer3D::SwitchGLSetting(GLenum id) const	// Switches setting value from on/off or viceversa
{
	if (glIsEnabled(id)) {
		glDisable(id);
		return false;
	}
	else {
		glEnable(id);
		return true;
	}	
}