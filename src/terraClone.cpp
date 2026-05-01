#include <iostream>
#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>

int main()
{
	const int SCR_WIDTH = 800;
	const int SCR_HEIGHT = 600;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(SCR_WIDTH, SCR_HEIGHT, "Raylib test window");

	rlImGuiSetup(true);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);

		rlImGuiBegin();

		DrawText("Test line of text.", 325, 100, 20, RED);

		ImGui::Begin("Test Widget");

		ImGui::Text("Hello");
		ImGui::Button("Button");

		ImGui::End();

		ImGui::ShowDemoWindow();

		rlImGuiEnd();

		EndDrawing();
	}

	rlImGuiShutdown();

	CloseWindow();

	return 0;
}