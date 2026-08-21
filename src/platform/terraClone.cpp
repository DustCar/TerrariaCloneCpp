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

	const int SCR_WIDTH = 1700;
	const int SCR_HEIGHT = 800;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(SCR_WIDTH, SCR_HEIGHT, "Raylib test window");
	SetExitKey(KEY_NULL);	// Disable ESC from closing window
	SetTargetFPS(240);

/* imgui setup begin */
	rlImGuiSetup(true);

	// set imgui flags
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	SetupImGuiAmethystStyle();
/* imgui setup end */

	if (!initGame())
	{
		return 0;
	}

	while (!WindowShouldClose())
	{
		BeginDrawing();

	/* imgui setup begin */
		rlImGuiBegin();

		// removes gray box that covers the game screen
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		// allows docking widgets to main window; Passthru to allow input to pass to the main window
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::PopStyleColor(2);
	/* imgui setup end */

		
		if (!updateGame())
		{
			CloseWindow();
		}

		rlImGuiEnd();

		EndDrawing();
	}

	CloseWindow();

	closeGame();

	rlImGuiShutdown();


	return 0;
}
