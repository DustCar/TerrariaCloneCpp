#include <raylib.h>
#include <iostream>
#include "gameMain.h"


// sure this is a global struct, but it's only global in this cpp file
struct GameData
{
	Texture dirtTexture;
}gameData;

bool initGame()
{
	gameData.dirtTexture = LoadTexture(RESOURCES_PATH "dirt.png");

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	// cap deltaTime at 5 frames per second
	if (deltaTime > 1.f / 5.f) { deltaTime = 1.f / 5.f; }

	DrawTexturePro(gameData.dirtTexture, { 0,0,(float)gameData.dirtTexture.width,(float)gameData.dirtTexture.height },
		{ 100,50,100,100 }, {}, 0, WHITE);

	return true;
}

void closeGame()
{

}

