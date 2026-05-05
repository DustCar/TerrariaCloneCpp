#include <iostream>
#include <sstream>
#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>

#pragma region imgui functions
static void ShowCherryWindow(bool* p_open);
#pragma endregion

int main()
{
	const int SCR_WIDTH = 800;
	const int SCR_HEIGHT = 600;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(SCR_WIDTH, SCR_HEIGHT, "Raylib test window");

	rlImGuiSetup(true);

#pragma region imgui
	// set imgui flags
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.FontGlobalScale = 1;
#pragma endregion

	while (!WindowShouldClose())
	{

		BeginDrawing();
		ClearBackground(RAYWHITE);

	#pragma region imgui
		rlImGuiBegin();

		// removes gray box that covers the game screen
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		// allows docking widgets to main window
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);
	#pragma endregion

		DrawText("Test line of text.", 325, 100, 20, RED);

	#pragma region imgui windows
		//ImGui::ShowDemoWindow();

		static bool show_cherry_window = false;

		if (show_cherry_window)		ShowCherryWindow(&show_cherry_window);

		ImGui::Begin("Widgets Challenge", NULL, ImGuiWindowFlags_MenuBar);
		if (ImGui::IsWindowFocused())
		{
			if (IsKeyDown(KEY_LEFT_CONTROL))
			{
				if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_C))
				{
					show_cherry_window = !show_cherry_window;
				}
			}
		}
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				ImGui::MenuItem("Simple");
				if (ImGui::BeginMenu("NestedMenu"))
				{
					ImGui::MenuItem("NestedSimple");
					ImGui::MenuItem("Disabled", NULL, false, false);
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Numbers"))
				{
					static int vol = 0;
					ImGui::SliderInt("Volume", &vol, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
					ImGui::InputInt("Input", &vol, 10);
					vol = vol < 0 ? 0 : vol;
					vol = vol > 100 ? 100 : vol;

					static bool value_enable = true;
					ImGui::Checkbox("Enabled", &value_enable);
					if (ImGui::BeginMenu("Toggleable Menu", value_enable))
					{
						ImGui::MenuItem("Final");
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::MenuItem("Cherry", "CTRL+ALT+C", &show_cherry_window);
				ImGui::MenuItem("Durian", NULL, false, false);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (ImGui::CollapsingHeader("Random"))
		{
			ImGui::SeparatorText("RANDOM FACTS:");
			ImGui::BulletText("In Africa, every 60 seconds, 1 minute passes...");
			ImGui::BulletText("There are more grains of sand on earth than there are stars in space.");

			ImGui::SeparatorText("RANDOM GAMES:");
			ImGui::BulletText("Wuthering Waves");
			ImGui::BulletText("Warframe");
			ImGui::BulletText("Goddess of Victory: NIKKE");
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Tree Example"))
		{
			if (ImGui::TreeNode("Numbers"))
			{
				static float f_tree = 88.25f;
				ImGui::SliderFloat("Tree Slider", &f_tree, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Text"))
			{
				ImGui::Text("Normal Text");
				ImGui::BulletText("Bullet Text");

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::End();
	#pragma endregion

	#pragma region imgui
		rlImGuiEnd();
	#pragma endregion

		EndDrawing();
	}

#pragma region imgui
	rlImGuiShutdown();
#pragma endregion

	CloseWindow();

	return 0;
}

#pragma region imgui functions
void ShowCherryWindow(bool* p_open)
{
	if (!ImGui::Begin("Cherry Window", p_open))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Here is the cherry window");
	ImGui::Separator();

	static float progress = 0.0f, progress_dir = 1.0f;
	progress += progress_dir * 0.5f * GetFrameTime();
	if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
	if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
	ImGui::BeginChild("picker", ImVec2(0, 50), true);
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
	ImGui::EndChild();

	if (ImGui::CollapsingHeader("Numbers"))
	{
		static int val = 0;
		std::stringstream ss;
		ss << "Chosen value: " << val + 1;
		ImGui::Text(ss.str().c_str());

		ImGui::RadioButton("Number 1", &val, 0); ImGui::SameLine();
		ImGui::RadioButton("Number 2", &val, 1); ImGui::SameLine();
		ImGui::RadioButton("Number 3", &val, 2);

		ImGui::Separator();
		ImGui::NewLine();

		ImGuiSliderFlags sl_flags{};
		sl_flags |= ImGuiSliderFlags_AlwaysClamp;

		static float drag = 0.0f;
		ImGui::DragFloat("Drag Number", &drag, 0.1f, 0.0f, 10.f, "%.3f", sl_flags);

		ImGui::SeparatorText("SLIDER VARIANTS:");
		ImGui::NewLine();

		static float slider = 5.0f;
		ImGui::VSliderFloat("Vertical", ImVec2(50, 150), &slider, 0.0f, 10.0f, "%.1f", sl_flags);
	}

	if (ImGui::CollapsingHeader("Color"))
	{
		static float color[4] = { 1.0f, 0.5f, 0.75f, 1.0f };
		ImGui::ColorEdit3("Color", color);
		ImGui::ColorPicker4("Color", color);
	}

	ImGui::End();
}
#pragma endregion

