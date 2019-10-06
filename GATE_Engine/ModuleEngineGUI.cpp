#include "ModuleEngineGUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"

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

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	bool show_another_window = false;
	
	return true;
}

bool ModuleEngineGUI::Start()
{
	
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
		show_configuration_window = !show_configuration_window;

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

	//Main toolbar
	if (ImGui::BeginMainMenuBar()) {

		// File: Options for file and App management
		if (ImGui::BeginMenu("File", true)) {

			if (ImGui::BeginMenu("New Scene", true)) {

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Open Scene", true)) {

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("Save", true)) {

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Save As...", true)) {

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::BeginMenu("New Project", true)) {

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Open Project", true)) {

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Save Project", true)) {

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit", "(Alt+F4)", false, true)) {

				return update_status::UPDATE_STOP;
			}

			ImGui::EndMenu();
		}

		// Edit: Tools for easier edition
		if (ImGui::BeginMenu("Edit", true)) {

			ImGui::EndMenu();
		}
		
		// Assets: Create premade objects and primitives
		if (ImGui::BeginMenu("Assets", true)) {

			ImGui::EndMenu();
		}

		// View: Display options
		if (ImGui::BeginMenu("View", true)) {

			if (ImGui::MenuItem("Show Console", "1"))
				show_console_window = !show_console_window;

			if (ImGui::MenuItem("Show Configuration", "4"))
				show_configuration_window = !show_configuration_window;

			if (ImGui::BeginMenu("DevTools")) {
				if (ImGui::MenuItem("Show UI Demo"))
					show_demo_window = !show_demo_window;

				if (ImGui::MenuItem("Generate Random Game"))
					App->RequestBrowser("https://www.youtube.com/watch?v=dQw4w9WgXcQ");

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		// Help: Information & Documentation
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::Button("About GATE...")) {
				show_about_window = true;
				ImGui::OpenPopup("About this engine...");
			}

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
				if (ImGui::SmallButton("SDL (v2.0)")) {
					App->RequestBrowser("https://www.libsdl.org/");
				}
				ImGui::Bullet();
				if (ImGui::SmallButton("OpenGL (v3.1.0)")) {
					App->RequestBrowser("https://www.opengl.org/");
				}
				ImGui::Bullet();
				if (ImGui::SmallButton("Brofiler (v1.1.2)")) {
					App->RequestBrowser("http://www.brofiler.com/");
				}
				ImGui::Bullet();
				if (ImGui::SmallButton("Dear ImGui (v1.72b)")) {
					App->RequestBrowser("https://github.com/ocornut/imgui");
				}
				ImGui::Bullet();
				if (ImGui::SmallButton("JSON for Modern C++ (v3.7.0)")) {
					App->RequestBrowser("https://github.com/nlohmann/json");
				}
				ImGui::Bullet();
				if (ImGui::SmallButton("MathGeoLib (v1.5)")) {
					App->RequestBrowser("https://github.com/juj/MathGeoLib");
				}
				ImGui::Bullet();
				if (ImGui::SmallButton("glew (v2.0)")) {
					App->RequestBrowser("http://glew.sourceforge.net/");
				}
				ImGui::Bullet();
				if (ImGui::SmallButton("mmgr")) {
					App->RequestBrowser("http://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml");
				}

				ImGui::Separator();

				ImGui::PushTextWrapPos(600.0f);
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
	}

	ImGui::EndMainMenuBar();

	//TEST IMGUI CONSOLE
	if(show_imgui_console)
	console.Draw("GATE Console",&show_imgui_console);

	//CONSOLE MENU
	if (show_console_window)
	{
		ImGui::Begin("Console",&show_console_window);

		for (int i = 0; i < App->console_LOG.size(); ++i)
			ImGui::Text(App->console_LOG[i].data());

		ImGui::End();

	}

	//CONFIGURATION MENU	
	if (show_configuration_window)
	{
		ImGui::Begin("Configuration", &show_configuration_window);

		if (ImGui::BeginMenu("Options"))
		{
			ImGui::MenuItem("Set Defaults");
			ImGui::MenuItem("Save");
			ImGui::MenuItem("Load");

			ImGui::EndMenu();
		}

		if (ImGui::CollapsingHeader("Application"))
		{
			static char str0[128] = "Add name and functions!";
			ImGui::InputText("App Name", str0, IM_ARRAYSIZE(str0));

			static char str1[128] = "Add organization and functions!";
			ImGui::InputText("Organization", str1, IM_ARRAYSIZE(str1));

			ImGui::SliderInt("MAX FPS", &App->max_FPS, -1, 120);

			//Plotting FPS and ms
			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", App->fps_log[App->fps_log.size() - 1]);
			ImGui::PlotHistogram("##framerate", &App->fps_log[0], App->fps_log.size(), 0, title, 0.0f, 140.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %.1f", App->ms_log[App->ms_log.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &App->ms_log[0], App->ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			ImGui::Text("Icon:");

			if (ImGui::SliderFloat("Brightness", &App->window->window_brightness, 0.000f, 1.000f))
				App->window->ChangeWindowBrightnessTo(App->window->window_brightness);

			if (ImGui::SliderInt("Width", &App->window->window_width, 256, 4096))
				App->window->ResizeWindow(App->window->window_width, App->window->window_height);

			if (ImGui::SliderInt("Height", &App->window->window_height, 144, 2160))
				App->window->ResizeWindow(App->window->window_width, App->window->window_height);

			ImGui::Text("Refresh rate: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%.1f", App->fps_log[App->fps_log.size() - 1]);


			//Fullscreen
			if (ImGui::Checkbox("Fullscreen", &App->window->window_fullscreen))
				App->window->WindowSetFullscreen(App->window->window_fullscreen);

			ImGui::SameLine();

			if (ImGui::Checkbox("Resizable", &App->window->window_resizable))
				App->window->WindowSetResizable(App->window->window_resizable);

			if (ImGui::Checkbox("Borderless", &App->window->window_borderless))
				App->window->WindowSetBorderless(App->window->window_borderless);

			ImGui::SameLine();

			if (ImGui::Checkbox("Full Desktop", &App->window->window_full_desktop))
				App->window->WindowSetFullscreenDesktop(App->window->window_full_desktop);
		}

		if (ImGui::CollapsingHeader("Input"))
		{
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

		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImGui::Text("SDL Version: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(255.0f, 0.0f, 255.0f, 255.00f), "%d.%d.%d", (int)App->hardware.sdl_version.major, (int)App->hardware.sdl_version.minor, (int)App->hardware.sdl_version.patch);

			ImGui::Text("CPUs: %d", App->hardware.CPU_logic_cores);
			ImGui::Text("System RAM: %f Gb", (float)App->hardware.RAM);

			ImGui::Text("CPU has Features: ");
			for (int i = 0; i < App->CPU_features.size(); ++i)
			{
				ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.0f), "%s,", App->CPU_features[i].data());
				if (i == 0 || (float)(i % 4) != 0.0f)
					ImGui::SameLine();
			}

			ImGui::Separator();

			ImGui::Text("GPU: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%s", App->hardware.GPU.version);

			ImGui::Text("Brand: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%s %s", App->hardware.GPU.vendor, App->hardware.GPU.renderer);

			ImGui::Separator();

			ImGui::Text("VRAM Budget: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%d", App->hardware.GPU.VRAM.budget);

			ImGui::Text("VRAM Usage: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%d", App->hardware.GPU.VRAM.usage);

			ImGui::Text("VRAM Available: ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%d", App->hardware.GPU.VRAM.available);

			ImGui::Text("VRAM Reserved: ");
		}


		ImGui::End();
	}

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