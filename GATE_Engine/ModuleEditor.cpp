#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "GeometryLoader.h"

// Windows
#include "EditorConfiguration.h"
#include "EditorConsole.h"
#include "EditorGame.h"
#include "EditorHierarchy.h"
#include "EditorInspector.h"
#include "EditorProject.h"
#include "EditorScene.h"
#include "EditorToolbar.h"

// Elements
#include "EditorMenuBar.h"

// SDL
#include "libs/Brofiler/Brofiler.h"

// Memory Leak Detection
#include "MemLeaks.h"

ModuleEditor::ModuleEditor(Application * app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
	// Windows
	editor_configuration = new EditorConfiguration("Configuration", false, ImGuiWindowFlags_MenuBar);
	editor_console = new EditorConsole("Console", true);
	editor_game = new EditorGame("Game", false);
	editor_hierarchy = new EditorHierarchy("Hierarchy", true);
	editor_inspector = new EditorInspector("Inspector", true);
	editor_project = new EditorProject("Project", false);
	editor_scene = new EditorScene("Scene", false);
	editor_toolbar = new EditorToolbar("Toolbar", false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

	// Elements
	editor_menu_bar = new EditorMenuBar();

#ifndef _DEBUG
	ui_debug_mode = false;
#endif
}

ModuleEditor::~ModuleEditor()
{
	// Windows
	RELEASE(editor_configuration);
	RELEASE(editor_console);
	RELEASE(editor_game);
	RELEASE(editor_hierarchy);
	RELEASE(editor_inspector);
	RELEASE(editor_project);
	RELEASE(editor_scene);
	RELEASE(editor_toolbar);

	// Elements
	RELEASE(editor_menu_bar);
}

bool ModuleEditor::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;

	//io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	/*ImGuiStyle& style = ImGui::GetStyle();
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}*/

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	bool show_another_window = false;	//CHANGE/FIX: Remove
	
	return true;
}

bool ModuleEditor::Start()
{
	// After loading the mode value (integer ID) from JSON, we actually apply the needed changes depending on the current mode
	DrawModeChange();
	TextureModeChange();
	ByteSizeModeChange();

	return true;
}

update_status ModuleEditor::Update(float dt)
{
	BROFILER_CATEGORY("Renderer pre-Update", Profiler::Color::Green);

	//Before we start with ImGui menus & stuff, we check for windows and menus that can be opened or closed with keyboard keys

	// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
	}

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (BeginRootWindow("Root Window 1", main_dockSpace, ImGuiWindowFlags_MenuBar)) {	//CHANGE/FIX: Create a root window class with docking that can be turned on/off?
		editor_menu_bar->Update();
		ImGui::End();
	}

	// Check all windows for updates if they're activated
	editor_toolbar->RequestUpdate();
	editor_configuration->RequestUpdate();
	editor_console->RequestUpdate();
	editor_game->RequestUpdate();
	editor_hierarchy->RequestUpdate();
	editor_inspector->RequestUpdate();
	editor_project->RequestUpdate();
	editor_scene->RequestUpdate();

	// Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()!
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	//If we are using a Imgui Menu, we update the bool that indicates so
	 using_menu = io->WantCaptureMouse;

	return UPDATE_CONTINUE;
}

bool ModuleEditor::BeginRootWindow(char* id, bool docking, ImGuiWindowFlags winFlags)
{
	ImGuiViewport* viewport = ImGui::GetWindowViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	winFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	winFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	winFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

	bool temp = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	temp = ImGui::Begin(id, &temp, winFlags);
	ImGui::PopStyleVar(3);

	if (docking) {
		BeginDockingSpace(id, ImGuiDockNodeFlags_PassthruCentralNode);
		//ImGui::DockSpaceOverViewport(viewport, ImGuiDockNodeFlags_PassthruCentralNode);	//Note: This is an automated way of doing the same, but lacks the capability of having a menuBar
	}

	return temp;
}

void ModuleEditor::BeginDockingSpace(char* dockSpaceId, ImGuiDockNodeFlags dockFlags, ImVec2 size)
{
	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID dockspace_id = ImGui::GetID(dockSpaceId);
		ImGui::DockSpace(dockspace_id, size, dockFlags);
	}
}

// ---------------------------------

void ModuleEditor::RenderEditorUI()
{
	ImGuiIO& test_io = *io;
	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)test_io.DisplaySize.x, (int)test_io.DisplaySize.y);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);	//DIDAC/CARLES: This line renders a plain color over the axis + grid plane of SceneIntro Module
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Modes & Radio Buttons
void ModuleEditor::DrawModeChange()
{
	switch (drawMode) {
	case (int)draw_mode::MESH:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case (int)draw_mode::WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case (int)draw_mode::VERTEX:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	}
}

void ModuleEditor::TextureModeChange()
{
	switch (textureMode) {
	case (int)texture_mode::TWO_D:
		App->renderer3D->SwitchGroupGLSetting(App->renderer3D->GL_Texture2D, App->renderer3D->GL_Texture2D.group);
		break;
	case (int)texture_mode::CUBEMAP:
		App->renderer3D->SwitchGroupGLSetting(App->renderer3D->GL_TextureCubeMap, App->renderer3D->GL_TextureCubeMap.group);
		break;
	}
}

void ModuleEditor::ByteSizeModeChange()
{
	switch (byteSizeMode) {
	case (int)byte_size_mode::KB:
		byteAlt = 1.0;
		byteText.assign("KB");
		break;
	case (int)byte_size_mode::MB:
		byteAlt = 1.0 / 1024.0;
		byteText.assign("MB");
		break;
	case (int)byte_size_mode::GB:
		byteAlt = 1.0 / 1024.0 / 1024.0;
		byteText.assign("GB");
		break;
	}
}