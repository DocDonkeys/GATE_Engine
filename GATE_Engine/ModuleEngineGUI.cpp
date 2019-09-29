#include "ModuleEngineGUI.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

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
	io = &ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	return true;
}

bool ModuleEngineGUI::Start()
{

	return true;
}

update_status ModuleEngineGUI::Update(float dt)
{
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
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

		if (ImGui::BeginMenu("File", true)) {

			if (ImGui::MenuItem("Exit", "(Esc)", false, true)) {

				return update_status::UPDATE_STOP;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools", true)) {

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			//if (ImGui::MenuItem("Gui Demo"))

			if (ImGui::MenuItem("Documentation"))
				App->RequestBrowser("https://github.com/DocDonkeys/GATE/wiki");

			if (ImGui::MenuItem("Download latest"))
				App->RequestBrowser("https://github.com/DocDonkeys/GATE/releases");

			if (ImGui::MenuItem("Report a Bug / Suggest an improvement"))
				App->RequestBrowser("https://github.com/DocDonkeys/GATE/issues");

			//if (ImGui::MenuItem("About"))
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	//CONFIGURATION MENU	
	ImGui::Begin("Configuration", open_configuration);

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
		sprintf_s(title, 25, "Framerate %.1f",App->fps_log[App->fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &App->fps_log[0], App->fps_log.size(), 0, title, 0.0f, 140.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", App->ms_log[App->ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &App->ms_log[0], App->ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::Text("Icon:");

		if (ImGui::SliderFloat("Brightness", &window_brightness, 0.000f, 1.000f))
			App->window->ChangeWindowBrightnessTo(window_brightness);

		if (ImGui::SliderInt("Width", &window_width, 256, 4096))
			App->window->ResizeWindow(window_width,window_height);

		if (ImGui::SliderInt("Height", &window_height, 144, 2160))
			App->window->ResizeWindow(window_width, window_height);

		ImGui::Text("Refresh rate: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%.1f", App->fps_log[App->fps_log.size() - 1]);


		//Fullscreen
		if (ImGui::Checkbox("Fullscreen", &window_fullscreen))
			App->window->WindowSetFullscreen(window_fullscreen);

		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &window_resizable))
			App->window->WindowSetResizable(window_resizable);

		if (ImGui::Checkbox("Borderless", &window_borderless))
			App->window->WindowSetBorderless(window_borderless);

		ImGui::SameLine();

		if(ImGui::Checkbox("Full Desktop", &window_full_desktop))
			App->window->WindowSetFullscreenDesktop(window_full_desktop);
	}

	if (ImGui::CollapsingHeader("Input"))
	{
	}

	if (ImGui::CollapsingHeader("Hardware"))
	{
		ImGui::Text("SDL Version: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(255.0f, 0.0f, 255.0f, 255.00f), "%d.%d.%d", (int)App->hardware.sdl_version.major, (int)App->hardware.sdl_version.minor, (int)App->hardware.sdl_version.patch);

		ImGui::Text("CPUs: %d", App->hardware.CPU_logic_cores);
		ImGui::Text("System RAM: %f Gb", (float)App->hardware.RAM);

		ImGui::NewLine();

		ImGui::Text("GPU: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%s", App->hardware.GPU.version);

		ImGui::Text("Brand: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 255.0f, 0.0f, 255.00f), "%s %s", App->hardware.GPU.vendor, App->hardware.GPU.renderer);

		ImGui::NewLine();

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

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	ImGuiIO& test_io = *io;
	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)test_io.DisplaySize.x, (int)test_io.DisplaySize.y);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);	//DIDAC/CARLES: This line renders a plain color over the axis + grid plane of SceneIntro Module
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}
