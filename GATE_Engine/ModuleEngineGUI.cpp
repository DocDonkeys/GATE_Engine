#include "ModuleEngineGUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"


//#pragma comment (lib, "imgui-1.72b/examples/libs/glfw/lib-vc2010-32/glfw3.lib")


ModuleEngineGUI::ModuleEngineGUI(Application * app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleEngineGUI::~ModuleEngineGUI()
{
}

bool ModuleEngineGUI::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();
	

	return true;
}

bool ModuleEngineGUI::Start()
{
	
	return true;
}

update_status ModuleEngineGUI::Update(float dt)
{


	return UPDATE_CONTINUE;
}
