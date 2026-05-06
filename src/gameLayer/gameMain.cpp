#include <raylib.h>
#include <iostream>
#include "gameMain.h"


// sure this is a global struct, but it's only global in this cpp file
struct GameData
{
	float positionX = 100;
	float positionY = 100;
}gameData;

bool initGame()
{
	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	// cap deltaTime at 5 frames per second
	if (deltaTime > 1.f / 5.f) { deltaTime = 1.f / 5.f; }

	if (IsKeyDown(KEY_A)) { gameData.positionX -= 200.f * deltaTime; }
	if (IsKeyDown(KEY_D)) { gameData.positionX += 200.f * deltaTime; }
	if (IsKeyDown(KEY_W)) { gameData.positionY -= 200.f * deltaTime; }
	if (IsKeyDown(KEY_S)) { gameData.positionY += 200.f * deltaTime; }

	DrawRectangle(gameData.positionX, gameData.positionY, 50, 50, BLUE);

	return true;
}

void closeGame()
{

}

