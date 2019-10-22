#include "ModuleEngineGUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "GeometryLoader.h"

ModuleEngineGUI::ModuleEngineGUI(Application * app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{
}

ModuleEngineGUI::~ModuleEngineGUI()
{
}

bool ModuleEngineGUI::Init()
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

bool ModuleEngineGUI::Start()
{
	// After loading the mode value (integer ID) from JSON, we actually apply the needed changes depending on the current mode
	DrawModeChange();
	TextureModeChange();
	ByteSizeModeChange();

	return true;
}

update_status ModuleEngineGUI::Update(float dt)
{
	BROFILER_CATEGORY("Renderer pre-Update", Profiler::Color::Green);

	//Before we start with ImGui menus & stuff, we check for windows and menus that can be opened or closed with keyboard keys


	//Console 1
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		show_console_window = !show_console_window;
	//Configuration 4
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		show_settings_window = !show_settings_window;

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

	if (BeginRootWindow("Root Window 1", main_dockSpace, ImGuiWindowFlags_MenuBar)) {	//CHANGE/FIX: Make so that root windows have an editable bool docking value

		//Main toolbar
		if (ImGui::BeginMenuBar()) {

			// Menu - File: Options for file and App management
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("New Scene", "Ctrl+N")) {

				}

				if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Save", "Ctrl+S")) {

				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {

				}

				ImGui::Separator();

				if (ImGui::MenuItem("New Project", "Ctrl+Shift+N")) {

				}

				if (ImGui::MenuItem("Open Project", "Ctrl+Shift+O")) {

				}

				if (ImGui::MenuItem("Save Project")) {

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit", "(Alt+F4)", false)) {

					return update_status::UPDATE_STOP;
				}

				ImGui::EndMenu();
			}

			// Menu - Edit: Tools for easier edition
			if (ImGui::BeginMenu("Edit")) {

				if (ImGui::MenuItem("Undo", "Ctrl+Z")) {

				}

				if (ImGui::MenuItem("Redo", "Ctrl+Y")) {

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Select All", "Ctrl+A")) {

				}

				if (ImGui::MenuItem("Deselect All", "Shift+D")) {

				}

				if (ImGui::MenuItem("Select Children", "Shift+C")) {

				}

				if (ImGui::MenuItem("Invert Selection", "Ctrl+I")) {

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {

				}

				if (ImGui::MenuItem("Delete", "Supr")) {

				}

				ImGui::Separator();

				if (ImGui::MenuItem("Play", "Ctrl+P")) {

				}

				if (ImGui::MenuItem("Pause", "Ctrl+Shift+P")) {

				}

				if (ImGui::MenuItem("Step", "Ctrl+Alt+P")) {

				}

				ImGui::Separator();

				// MenuItem - Settings: Engine confiuration settings
				if (ImGui::MenuItem("Settings...")) {
					show_settings_window = !show_settings_window;
				}

				ImGui::EndMenu();
			}

			// Menu - Window: View options
			if (ImGui::BeginMenu("Window")) {

				if (ImGui::BeginMenu("Viewport")) {

					//ImGui::Checkbox("Framerate", false);
					//ImGui::Checkbox("Polycount", false);
					//ImGui::Checkbox("Base Grid", false);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Menus")) {

					ImGui::MenuItem("Heriarchy", NULL, &show_heriarchy_window);
					ImGui::MenuItem("Project", NULL, &show_project_window);
					ImGui::MenuItem("Console", NULL, &show_imgui_console);
					ImGui::MenuItem("Inspector", NULL, &show_inspector_window);
					ImGui::MenuItem("Scene", NULL, &show_scene_window);
					ImGui::MenuItem("Game", NULL, &show_game_window);

					ImGui::EndMenu();
				}


				ImGui::Separator();

				// Menu: Engine Development Tools
				if (ImGui::BeginMenu("DevDebug")) {
					if (ImGui::MenuItem("Show UI Demo"))
						show_demo_window = !show_demo_window;

					if (ImGui::MenuItem("Generate Random Game"))
						App->RequestBrowser("https://www.youtube.com/watch?v=dQw4w9WgXcQ");

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			// Menu - GameObjects: Create premade objects and primitives
			if (ImGui::BeginMenu("GameObjects")) {

				if (ImGui::BeginMenu("Create...")) {

					if (ImGui::MenuItem("Plane"))
					{
						App->geometry_loader->CreatePrimitive(PRIMITIVE::PLANE, 10, 10);
					}

					if (ImGui::MenuItem("Cube"))
					{
						App->geometry_loader->CreatePrimitive(PRIMITIVE::CUBE);
					}

					if (ImGui::MenuItem("Sphere"))
					{
						App->geometry_loader->CreatePrimitive(PRIMITIVE::SPHERE, 30, 30);
					}

					if (ImGui::MenuItem("Hemisphere"))
					{
						App->geometry_loader->CreatePrimitive(PRIMITIVE::HEMISPHERE, 30, 30);
					}

					if (ImGui::MenuItem("Cylinder"))
					{
						App->geometry_loader->CreatePrimitive(PRIMITIVE::CYLINDER, 30, 30);
					}

					if (ImGui::MenuItem("Cone"))
					{
						App->geometry_loader->CreatePrimitive(PRIMITIVE::CONE, 30, 30);
					}

					if (ImGui::MenuItem("Torus"))
					{
						App->geometry_loader->CreatePrimitive(PRIMITIVE::TORUS, 30, 30, 0.5f);
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Draw Mode")) {

					if (ImGui::RadioButton("Mesh", &drawMode, (int)draw_mode::MESH)
						|| ImGui::RadioButton("Wireframe", &drawMode, (int)draw_mode::WIREFRAME)
						|| ImGui::RadioButton("Vertexs", &drawMode, (int)draw_mode::VERTEX))
					{
						DrawModeChange();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			// Menu - Help: Information & Documentation
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::Button("About GATE...")) {
					show_about_window = true;
					ImGui::OpenPopup("About this engine...");
				}

				// PopupModal - About: Information about the engine and its properties
				if (ImGui::BeginPopupModal("About this engine...", &show_about_window, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("%s\nA simple engine for 3D game development.\nBy %s, %s", App->GetTitle(), App->GetAuthors(), App->GetOrganization());
					ImGui::Separator();

					ImGui::Text("3rd Party Libraries used:\n");
					ImGui::Bullet();
					if (ImGui::SmallButton("STL (C++11)")) {
						App->RequestBrowser("https://www.geeksforgeeks.org/the-c-standard-template-library-stl/");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("SDL (v2.0.10)")) {
						App->RequestBrowser("https://www.libsdl.org/");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("OpenGL (v3.1.0)")) {
						App->RequestBrowser("https://www.opengl.org/");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("DevIL (v1.8.0)")) {
						App->RequestBrowser("http://openil.sourceforge.net/");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("Assimp (v5.0.0)")) {
						App->RequestBrowser("http://assimp.org/");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("Dear ImGui (v1.73, Base Code)")) {
						App->RequestBrowser("https://github.com/ocornut/imgui");
					} ImGui::SameLine();
					if (ImGui::SmallButton("Docking Test Branch (Commit 18/10/19)")) {
						App->RequestBrowser("https://github.com/ocornut/imgui/tree/7feccf9ab2fad261aa873dfa067e64ad9fab8a03");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("glew (v2.0)")) {
						App->RequestBrowser("http://glew.sourceforge.net/");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("MathGeoLib (v1.5)")) {
						App->RequestBrowser("https://github.com/juj/MathGeoLib");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("Par (vN/A)")) {
						App->RequestBrowser("https://github.com/prideout/par");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("JSON for Modern C++ (v3.7.0)")) {
						App->RequestBrowser("https://github.com/nlohmann/json");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("Brofiler (v1.1.2)")) {
						App->RequestBrowser("http://www.brofiler.com/");
					}
					ImGui::Bullet();
					if (ImGui::SmallButton("mmgr (vN/A)")) {
						App->RequestBrowser("http://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml");
					}

					ImGui::Separator();

					ImGui::PushTextWrapPos(standard_text_width);
					ImGui::Text(App->GetLicense());
					ImGui::PopTextWrapPos();

					ImGui::EndPopup();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Documentation"))
					App->RequestBrowser("https://github.com/DocDonkeys/GATE/wiki");

				if (ImGui::MenuItem("Download Latest Version"))
					App->RequestBrowser("https://github.com/DocDonkeys/GATE/releases");

				ImGui::Separator();

				if (ImGui::MenuItem("Report Bug / Suggest Feature"))
					App->RequestBrowser("https://github.com/DocDonkeys/GATE/issues");

				if (ImGui::MenuItem("Give us Feedback!"))
					App->RequestBrowser("https://www.youtube.com/watch?v=8o6c1UuoMwI");

				if (ImGui::MenuItem("Contact us!"))
					App->RequestBrowser("https://github.com/DocDonkeys/");



				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	// Main Windows
	if (show_imgui_console)	//TEST IMGUI CONSOLE	//CHANGE/FIX: Should we improve this?
		console.Draw("GATE Console", &show_imgui_console);

	if (show_heriarchy_window) {
		ImGui::Begin("Heriarchy", &show_heriarchy_window);

		ImGui::End();
	}

	if (show_project_window) {
		ImGui::Begin("Project", &show_project_window);

		ImGui::End();
	}

	if (show_inspector_window) {
		ImGui::Begin("Inspector", &show_inspector_window);

		ImGui::End();
	}

	if (show_scene_window) {
		ImGui::Begin("Scene", &show_scene_window);

		ImGui::End();
	}

	if (show_game_window) {
		ImGui::Begin("Game", &show_game_window);

		ImGui::End();
	}
	
	// Window - Configuration: Engine settings
	if (show_settings_window)
	{
		ImGui::Begin("Configuration", &show_settings_window, ImGuiWindowFlags_MenuBar);

		ImGui::BeginMenuBar();

		if (ImGui::MenuItem("Save")) {
			App->RequestConfigSave();
		}

		if (ImGui::MenuItem("Reset")) {	//CHANGE/FIX: Values are loaded and therefore reseted, but the changes they represent (like window size) don't
			App->RequestConfigLoad();
		}

		if (ImGui::MenuItem("Defaults")) {
			ImGui::OpenPopup("Are you sure?");
		}

		if (ImGui::BeginPopup("Are you sure?"))
		{
			ImGui::Text("Are you sure?");

			if (ImGui::Button("Yes")) {
				App->RequestConfigReset();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::MenuItem("Close")) {
			show_settings_window = false;
		}

		ImGui::EndMenuBar();

		if (ImGui::BeginTabBar("Setting Menus"))
		{
			// Application Tab
			//if (ImGui::BeginTabItem("Application"))	//CHANGE/FIX: Erase?
			//{
			//	ImGui::EndTabItem();
			//}

			// Display Tab
			if (ImGui::BeginTabItem("Display"))
			{
				//Window
				if (ImGui::CollapsingHeader("Window")) {
					if (ImGui::SliderInt("Width", &App->window->window_width, 256, 4096))
						App->window->ResizeWindow(App->window->window_width, App->window->window_height);

					if (ImGui::SliderInt("Height", &App->window->window_height, 144, 2160))
						App->window->ResizeWindow(App->window->window_width, App->window->window_height);

					if (ImGui::SliderFloat("Brightness", &App->window->window_brightness, 0.000f, 1.000f))
						App->window->ChangeWindowBrightnessTo(App->window->window_brightness);

					if (ImGui::Checkbox("Fullscreen", &App->window->window_fullscreen))
						App->window->WindowSetFullscreen(App->window->window_fullscreen);

					ImGui::SameLine();

					if (ImGui::Checkbox("Resizable", &App->window->window_resizable))
						App->window->WindowSetResizable(App->window->window_resizable);

					ImGui::SameLine();

					if (ImGui::Checkbox("Borderless", &App->window->window_borderless))
						App->window->WindowSetBorderless(App->window->window_borderless);

					ImGui::SameLine();

					if (ImGui::Checkbox("Full Desktop", &App->window->window_full_desktop))
						App->window->WindowSetFullscreenDesktop(App->window->window_full_desktop);
				}

				//Plotting FPS and ms
				if (ImGui::CollapsingHeader("Framerate")) {

					ImGui::SliderInt("MAX FPS", &App->max_FPS, -1, 120);
					ImGui::SameLine();
					if (ImGui::Checkbox("VSync", &App->renderer3D->vSync)) {
						if (App->renderer3D->vSync) {
							SDL_GL_SetSwapInterval(1);
						}
						else {
							SDL_GL_SetSwapInterval(0);
						}
					}

					char title[25];
					sprintf_s(title, 25, "Framerate %.1f", App->fps_log[App->fps_log.size() - 1]);
					ImGui::PlotHistogram("##framerate", &App->fps_log[0], App->fps_log.size(), 0, title, 0.0f, 140.0f, ImVec2(310, 100));
					sprintf_s(title, 25, "Milliseconds %.1f", App->ms_log[App->ms_log.size() - 1]);
					ImGui::PlotHistogram("##milliseconds", &App->ms_log[0], App->ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
				}
				
				ImGui::EndTabItem();
			}

			// Controls Tab
			if (ImGui::BeginTabItem("Controls"))
			{
				if (ImGui::CollapsingHeader("Camera")) {
					ImGui::Text("Keyboard Inputs");
					ImGui::SliderFloat("Movement Speed", &App->camera->camMovSpeed, App->camera->camDefaultMin, App->camera->maxMovSpeed);
					ImGui::SliderFloat("Movement Multiplier", &App->camera->camMovMultiplier, App->camera->camDefaultMin, App->camera->maxMovMultiplier);

					ImGui::SliderFloat("Rotation Speed", &App->camera->camRotSpeed, App->camera->camDefaultMin, App->camera->maxRotSpeed);
					ImGui::SliderFloat("Rotation Multiplier", &App->camera->camRotMultiplier, App->camera->camDefaultMin, App->camera->maxRotMultiplier);

					ImGui::Separator();

					ImGui::Text("Mouse Inputs");
					ImGui::SliderFloat("Mouse Sensibility", &App->camera->camMouseSens, App->camera->camDefaultMin, App->camera->maxMouseSens);
				}				

				if (ImGui::CollapsingHeader("Data")) {
					ImGui::Text("Mouse Position: "); ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.0f), "%d, %d", App->input->GetMouseX(), App->input->GetMouseY());

					ImGui::Text("Mouse Motion: "); ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.0f), "%d, %d", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());

					ImGui::Text("Mouse Wheel: "); ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.0f), "%d", App->input->GetMouseZ());

					ImGui::Text("Input Log");

					// Scrollbar TEST
					static bool disable_mouse_wheel = false;
					static bool disable_menu = false;
					ImGui::Checkbox("Disable Mouse Wheel", &disable_mouse_wheel);
					ImGui::Checkbox("Disable Menu", &disable_menu);

					static int line = 50;
					bool goto_line = ImGui::Button("Goto");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(100);
					goto_line |= ImGui::InputInt("##Line", &line, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);

					ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar | (disable_mouse_wheel ? ImGuiWindowFlags_NoScrollWithMouse : 0);
					ImGui::BeginChild("Child1", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 260), false, window_flags);
					if (App->input->input_log.size() > 0)
					{
						int i = 0;
						while (i != App->input->input_log.size())
						{
							//ImGui::Text("%04d: scrollable region", i);
							ImGui::Text("Key: %d %s", App->input->input_log[i], App->input->input_type_log[i]);
							if (goto_line && line == i)
								ImGui::SetScrollHereY();
							++i;
						}
						if (goto_line && line >= 100)
							ImGui::SetScrollHereY();
					}
					ImGui::EndChild();
				}

				ImGui::EndTabItem();
			}

			// Renderer Tab
			if (ImGui::BeginTabItem("Renderer")) {

				//Refresh rate
				ImGui::Text("Refresh rate: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%.1f", App->fps_log[App->fps_log.size() - 1]);

				ImGui::Separator();

				//GL_Settings
				ImGui::Text("OpenGL Options");

				ImGui::BulletText("General");

				if (ImGui::Checkbox("Depth Test", &App->renderer3D->GL_DepthTest.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_DepthTest);

				ImGui::SameLine();

				if (ImGui::Checkbox("Cull Face", &App->renderer3D->GL_CullFace.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_CullFace);

				ImGui::SameLine();

				if (ImGui::Checkbox("Lightning", &App->renderer3D->GL_Lighting.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_Lighting);

				//------------------------------------------------------------

				ImGui::BulletText("Textures");

				if (ImGui::RadioButton("2D", &textureMode, (int)texture_mode::TWO_D)) {
					TextureModeChange();
				}

				ImGui::SameLine();

				if (ImGui::RadioButton("Cube Map", &textureMode, (int)texture_mode::CUBEMAP)) {
					TextureModeChange();
				}

				//------------------------------------------------------------

				ImGui::BulletText("Points & Lines");

				if (ImGui::Checkbox("Point Smooth", &App->renderer3D->GL_PointSmooth.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_PointSmooth);

				ImGui::SameLine();

				if (ImGui::Checkbox("Line Smooth", &App->renderer3D->GL_LineSmooth.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_LineSmooth);

				ImGui::SameLine();

				if (ImGui::Checkbox("Line Stipple", &App->renderer3D->GL_LineStipple.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_LineStipple);

				//------------------------------------------------------------

				ImGui::BulletText("Polygons");

				if (ImGui::Checkbox("Polygon Smooth", &App->renderer3D->GL_PolygonSmooth.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_PolygonSmooth);

				ImGui::SameLine();

				if (ImGui::Checkbox("Polygon Stipple", &App->renderer3D->GL_PolygonStipple.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_PolygonStipple);

				//------------------------------------------------------------

				ImGui::BulletText("Color");

				if (ImGui::Checkbox("Blend", &App->renderer3D->GL_Blend.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_Blend);

				ImGui::SameLine();

				if (ImGui::Checkbox("Dither", &App->renderer3D->GL_Dither.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_Dither);

				//------------------------------------------------------------

				if (ImGui::Checkbox("Color Material", &App->renderer3D->GL_ColorMaterial.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_ColorMaterial);

				ImGui::SameLine();

				if (ImGui::Checkbox("MinMax", &App->renderer3D->GL_MinMax.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_MinMax);

				ImGui::SameLine();

				if (ImGui::Checkbox("Multi Sample", &App->renderer3D->GL_MultiSample.status))
					App->renderer3D->SwitchGLSetting(App->renderer3D->GL_MultiSample);

				ImGui::EndTabItem();
			}

			// Hardware Tab
			if (ImGui::BeginTabItem("System"))
			{
				if (ImGui::CollapsingHeader("Software")) {
					ImGui::Text("SDL: ");
					ImGui::BulletText("Version: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 0.0f, 255.0f, 255.00f), "%d.%d.%d", (int)App->hardware.sdl_version.major, (int)App->hardware.sdl_version.minor, (int)App->hardware.sdl_version.patch);

					ImGui::Text("OpenGL: ");
					ImGui::BulletText("Version: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%s", App->hardware.GPU.version);

					ImGui::BulletText("Vendor: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%s", App->hardware.GPU.vendor);

					ImGui::Text("DevIL: ");
					ImGui::BulletText("Version: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 0.0f, 0.0f, 255.00f), "%s", App->hardware.devil_info.version);

					ImGui::BulletText("Vendor: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 0.0f, 0.0f, 255.00f), "%s", App->hardware.devil_info.vendor);
				}

				if (ImGui::CollapsingHeader("Hardware")) {
					
					ImGui::Text("CPU: ");

					ImGui::BulletText("Cores: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.0f, 255.0f, 255.0f, 255.00f), "%d", App->hardware.CPU_logic_cores);

					ImGui::BulletText("Includes Features: ");
					for (int i = 0; i < App->CPU_features.size(); ++i)
					{
						ImGui::TextColored(ImVec4(0.0f, 255.0f, 255.0f, 255.0f), "%s,", App->CPU_features[i].data());
						if (i == 0 || (float)(i % 4) != 0.0f)
							ImGui::SameLine();
					}

					ImGui::Separator();

					ImGui::Text("GPU: ");
					ImGui::BulletText("Renderer: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%s", App->hardware.GPU.renderer);

					ImGui::Separator();

					ImGui::Text("RAM: ");

					ImGui::SameLine(250);
					if (ImGui::RadioButton("KB", &byteSizeMode, (int)byte_size_mode::KB)) {
						ByteSizeModeChange();
					}
					ImGui::SameLine();
					if (ImGui::RadioButton("MB", &byteSizeMode, (int)byte_size_mode::MB)) {
						ByteSizeModeChange();
					}
					ImGui::SameLine();
					if (ImGui::RadioButton("GB", &byteSizeMode, (int)byte_size_mode::GB)) {
						ByteSizeModeChange();
					}

					ImGui::BulletText("RAM: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%f %s", (float)(App->hardware.RAM * byteAlt), byteText.c_str());

					ImGui::Text("VRAM: ");
					ImGui::BulletText("VRAM Budget: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%f %s", (float)(App->hardware.GPU.VRAM.budget * byteAlt), byteText.c_str());

					ImGui::BulletText("VRAM Usage: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%f %s", (float)(App->hardware.GPU.VRAM.usage * byteAlt), byteText.c_str());

					ImGui::BulletText("VRAM Available: ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%f %s", (float)(App->hardware.GPU.VRAM.available * byteAlt), byteText.c_str());

					//ImGui::BulletText("VRAM Reserved: ");	//CHANGE/FIX: Until this has an actual value it will remain commented
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	//CONSOLE MENU
	//if (show_console_window)		//CHANGE/FIX: Delete, imgui console is the same but better
	//{
	//	ImGui::Begin("Console", &show_console_window);

	//	for (int i = 0; i < App->console_LOG.size(); ++i)
	//		ImGui::Text(App->console_LOG[i].data());

	//	ImGui::End();

	//}

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	return UPDATE_CONTINUE;
}

bool ModuleEngineGUI::BeginRootWindow(char* id, bool docking, ImGuiWindowFlags winFlags)
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

void ModuleEngineGUI::BeginDockingSpace(char* dockSpaceId, ImGuiDockNodeFlags dockFlags, ImVec2 size)
{
	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID dockspace_id = ImGui::GetID(dockSpaceId);
		ImGui::DockSpace(dockspace_id, size, dockFlags);
	}
}

// ---------------------------------

void ModuleEngineGUI::RenderGUI()
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
void ModuleEngineGUI::DrawModeChange()
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

void ModuleEngineGUI::TextureModeChange()
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

void ModuleEngineGUI::ByteSizeModeChange()
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