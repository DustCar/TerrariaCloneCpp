#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>
#include <helpers.h>


// sure this is a global struct, but it's only global in this cpp file
struct GameData
{
	GameMap gameMap;
	Camera2D camera;

}gameData;

AssetManager assetManager;

bool initGame()
{
	assetManager.loadAll();

	gameData.gameMap.create(30, 30);

	for (int y = 0; y < gameData.gameMap.h; y++)
		for (int x = 0; x < gameData.gameMap.w; x++)
		{
			float sinWave = std::sin(x) / 2.0f;
			float sinWave2 = std::sin(x * 0.5) / 2.0f;

			if (gameData.gameMap.h - (gameData.gameMap.h * sinWave * 0.3f) - gameData.gameMap.h * 0.5 - (gameData.gameMap.h * sinWave2 * 0.2f) < y)
			{
				gameData.gameMap.getBlockUnsafe(x, y).type = Block::dirt;
			}
			else
			{
				gameData.gameMap.getBlockUnsafe(x, y).type = Block::rubyBlock;
			}

		}

	gameData.camera.target = { 15,15 }; // world-space center of view; will be used as camera position
	gameData.camera.rotation = 0.0f;
	gameData.camera.zoom = 25.0f;

	return true;
}

bool updateGame()
{
	float deltaTime = GetFrameTime();
	// cap deltaTime at 5 frames per second
	if (deltaTime > 1.f / 5.f) { deltaTime = 1.f / 5.f; }

	gameData.camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };

	ClearBackground({ 75, 75, 150, 255 });
	
	if (IsKeyDown(KEY_A)) { gameData.camera.target.x -= 10.f * deltaTime; }
	if (IsKeyDown(KEY_D)) { gameData.camera.target.x += 10.f * deltaTime; }
	if (IsKeyDown(KEY_W)) { gameData.camera.target.y -= 10.f * deltaTime; }
	if (IsKeyDown(KEY_S)) { gameData.camera.target.y += 10.f * deltaTime; }

	BeginMode2D(gameData.camera);

	// y outer, x inner for right-down iteration like old tvs
	for (int y = 0; y < gameData.gameMap.h; y++)
		for (int x = 0; x < gameData.gameMap.w; x++)
		{
			// no need for checks, drawing whole map
			auto& b = gameData.gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air)
			{

				DrawTexturePro(
					assetManager.textures, // texture
					getTextureAtlas(b.type, 0, 32, 32), // source
					{ (float)x, (float)y, 1, 1 }, // dest
					{ 0, 0 }, // origin of texture (rotation/scale point) (top-left corner)
					0.f, // rotation
					WHITE // tint
				);
			}
		}

	EndMode2D();

	return true;
}

void closeGame()
{

}

