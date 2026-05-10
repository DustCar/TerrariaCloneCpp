#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>
#include <helpers.h>
#include <imgui.h>


// sure this is a global struct, but it's only global in this cpp file
struct GameData
{
	GameMap gameMap;
	Camera2D camera;

	int selectedBlock = Block::dirt;

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
				gameData.gameMap.getBlockUnsafe(x, y).type = Block::emerald;
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
	
#pragma region camera movement
	if (IsKeyDown(KEY_A)) { gameData.camera.target.x -= 10.f * deltaTime; }
	if (IsKeyDown(KEY_D)) { gameData.camera.target.x += 10.f * deltaTime; }
	if (IsKeyDown(KEY_W)) { gameData.camera.target.y -= 10.f * deltaTime; }
	if (IsKeyDown(KEY_S)) { gameData.camera.target.y += 10.f * deltaTime; }
#pragma endregion

	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	// break block
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
	{
		auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
		if (b)
		{
			*b = {};
		}
	}

	// place block
	if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	{
		auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
		// check adjacent blocks
		auto bUp = gameData.gameMap.getBlockSafe(blockX, blockY - 1);
		auto bDown = gameData.gameMap.getBlockSafe(blockX, blockY + 1);
		auto bRight = gameData.gameMap.getBlockSafe(blockX + 1, blockY);
		auto bLeft = gameData.gameMap.getBlockSafe(blockX - 1, blockY);

		// only place if the block can attach to another block and if space is empty
		if ((bUp && bUp->type != Block::air) || (bDown && bDown->type != Block::air) || (bRight && bRight->type != Block::air) || (bLeft && bLeft->type != Block::air))
		{
			if (b && b->type == Block::air)
			{
				b->type = gameData.selectedBlock;
			}
		}
		
	}

	BeginMode2D(gameData.camera);

	// y outer, x inner for right-down iteration like old tvs
	// draw blocks into world
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
					{ 0,0 }, // origin of texture (rotation/scale point) (top-left corner)
					0.f, // rotation
					WHITE // tint
				);
			}
		}

	// draw frame for block placement
	DrawTexturePro(
		assetManager.frame, // texture
		{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height }, // source
		{ (float)blockX, (float)blockY, 1, 1 }, // dest
		{ 0,0 }, // origin
		0.f, // rotation
		WHITE // tint
	);

	EndMode2D();

#pragma region ui

	ImGui::Begin("Block Selector");

	for (int i = 1; i < Block::BLOCKS_COUNT; i++)
	{
		auto atlas = getTextureAtlas(i, 0, 32, 32);
		// transform texture parameters into UV coords
		atlas.x /= assetManager.textures.width;
		atlas.width /= assetManager.textures.width;
		atlas.y /= assetManager.textures.height;
		atlas.height /= assetManager.textures.height;

		ImGui::PushID(i);

		ImTextureID tex = (ImTextureID)(intptr_t)assetManager.textures.id;
		if (ImGui::ImageButton("", tex, { 28, 28 },
			{ atlas.x, atlas.y }, { atlas.x + atlas.width, atlas.y + atlas.height }))
		{
			gameData.selectedBlock = i;
		}

		ImGui::PopID();

		if (i % 10 != 0)
		{
			ImGui::SameLine();
		}
	}

	ImGui::End();

#pragma endregion

	return true;
}

void closeGame()
{

}

