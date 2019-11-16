#include "EditorConfiguration.h"
#include "ModuleEditor.h"
#include "Application.h"

#ifdef _DEBUG
#ifdef _MMGR_MEM_LEAK
#include "libs/mmgr/mmgr.h"
#endif
#endif

EditorConfiguration::EditorConfiguration(const char* name, bool startEnabled, ImGuiWindowFlags flags) : EditorWindow(name, startEnabled, flags) {};

void EditorConfiguration::Update()
{
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
		show_window = false;
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

				ImGui::Separator();

				ImGui::Text("Mouse Inputs");
				ImGui::SliderFloat("Mouse Sensibility", &App->camera->mouseSens, App->camera->camDefaultMin, App->camera->maxMouseSens);
				ImGui::SliderFloat("Scroll Sensibility", &App->camera->scrollSens, App->camera->camDefaultMin, App->camera->maxScrollSens);
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

			if (ImGui::RadioButton("2D", &App->editor->textureMode, (int)texture_mode::TWO_D)) {
				App->editor->TextureModeChange();
			}

			ImGui::SameLine();

			if (ImGui::RadioButton("Cube Map", &App->editor->textureMode, (int)texture_mode::CUBEMAP)) {
				App->editor->TextureModeChange();
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
				if (ImGui::RadioButton("KB", &App->editor->byteSizeMode, (int)byte_size_mode::KB)) {
					App->editor->ByteSizeModeChange();
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("MB", &App->editor->byteSizeMode, (int)byte_size_mode::MB)) {
					App->editor->ByteSizeModeChange();
				}
				ImGui::SameLine();
				if (ImGui::RadioButton("GB", &App->editor->byteSizeMode, (int)byte_size_mode::GB)) {
					App->editor->ByteSizeModeChange();
				}

				ImGui::BulletText("RAM: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%f %s", (float)(App->hardware.RAM * App->editor->byteAlt), App->editor->byteText.c_str());

				ImGui::Text("VRAM: ");
				ImGui::BulletText("VRAM Budget: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%f %s", (float)(App->hardware.GPU.VRAM.budget * App->editor->byteAlt), App->editor->byteText.c_str());

				ImGui::BulletText("VRAM Usage: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%f %s", (float)(App->hardware.GPU.VRAM.usage * App->editor->byteAlt), App->editor->byteText.c_str());

				ImGui::BulletText("VRAM Available: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255.0f, 255.0f, 0.0f, 255.00f), "%f %s", (float)(App->hardware.GPU.VRAM.available * App->editor->byteAlt), App->editor->byteText.c_str());

				//ImGui::BulletText("VRAM Reserved: ");	//CHANGE/FIX: Until this has an actual value it will remain commented
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}