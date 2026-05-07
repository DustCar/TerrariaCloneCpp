#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>


// sure this is a global struct, but it's only global in this cpp file
struct GameData
{
}gameData;

AssetManager assetManager;

bool initGame()
{
	assetManager.loadAll();

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	// cap deltaTime at 5 frames per second
	if (deltaTime > 1.f / 5.f) { deltaTime = 1.f / 5.f; }

	DrawTexturePro(assetManager.castle, { 96,0,32,32 },
		{ 100,50,100,100 }, {}, 0, WHITE);

	return true;
}

void closeGame()
{

}

