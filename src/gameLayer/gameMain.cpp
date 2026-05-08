#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>


// sure this is a global struct, but it's only global in this cpp file
struct GameData
{
	GameMap gameMap;

}gameData;

AssetManager assetManager;

bool initGame()
{
	assetManager.loadAll();

	gameData.gameMap.create(30, 10);

	gameData.gameMap.getBlockUnsafe(0, 0).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(1, 1).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(2, 2).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(3, 3).type = Block::dirt;
	gameData.gameMap.getBlockUnsafe(4, 4).type = Block::dirt;

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	// cap deltaTime at 5 frames per second
	if (deltaTime > 1.f / 5.f) { deltaTime = 1.f / 5.f; }

	ClearBackground({ 75, 75, 150, 255 });

	// y outer, x inner for right-down iteration like old tvs
	for (int y = 0; y < gameData.gameMap.h; y++)
		for (int x = 0; x < gameData.gameMap.w; x++)
		{
			// no need for checks, drawing whole map
			auto& b = gameData.gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air)
			{
				float size = 32;
				// position in pixels
				float posX = x * size;
				float posY = y * size;

				DrawTexturePro(
					assetManager.dirt, // texture
					Rectangle{ 0.f, 0.f, (float)assetManager.dirt.width, (float)assetManager.dirt.height }, // source
					{ posX, posY, size, size }, // dest
					{ 0, 0 }, // origin in texture (top-left corner)
					0.f, // rotation
					WHITE // tint
				);


			}
		}

	return true;
}

void closeGame()
{

}

