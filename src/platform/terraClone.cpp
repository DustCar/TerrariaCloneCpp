#include <iostream>
#include <sstream>
#include <raylib.h>
#include <gameMain.h>
#include <asserts.h>

#include <imgui.h>
#include <rlImGui.h>
#include <imguiThemes.h>

int main()
{

#if PRODUCTION_BUILD == 1
	SetTraceLogLevel(LOG_NONE);
#endif

	const int SCR_WIDTH = 800;
	const int SCR_HEIGHT = 600;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(SCR_WIDTH, SCR_HEIGHT, "Raylib test window");
	SetExitKey(KEY_NULL);	// Disable ESC from closing window
	SetTargetFPS(240);

#pragma region imgui setup
	rlImGuiSetup(true);

	// set imgui flags
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.FontGlobalScale = 1;
#pragma endregion

	if (!initGame())
	{
		return 0;
	}

	while (!WindowShouldClose())
	{

		BeginDrawing();
		ClearBackground(BLACK);

	#pragma region imgui setup
		rlImGuiBegin();

		// removes gray box that covers the game screen
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		// allows docking widgets to main window
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);
	#pragma endregion

		if (!updateGame())
		{
			CloseWindow();
		}

	#pragma region imgui
		rlImGuiEnd();
	#pragma endregion

		EndDrawing();
	}

	CloseWindow();

	closeGame();

#pragma region imgui
	rlImGuiShutdown();
#pragma endregion


	return 0;
}
