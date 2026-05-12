#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>
#include <helpers.h>
#include <imgui.h>
#include <raymath.h>


// sure this is a global struct, but it's only global in this cpp file
struct GameData
{
	GameMap gameMap;
	Camera2D camera = Camera2D{};

	int selectedBlock = Block::dirt;

}gameData;

AssetManager assetManager;

bool initGame()
{
	assetManager.loadAll();

	gameData.gameMap.create(100, 30);

	for (int y = 0; y < gameData.gameMap.h; y++)
		for (int x = 0; x < gameData.gameMap.w; x++)
		{
			float sinWave = std::sin(0.25f * x) / 6.0f;
			float sinWave2 = std::sin(0.3f * x - 1.0f) / 2.0f;

			float surface = gameData.gameMap.h - (gameData.gameMap.h * sinWave * 0.1f) - gameData.gameMap.h * 0.5 - (gameData.gameMap.h * sinWave2 * 0.1f);
			
			if (y == (int)surface)
			{
				gameData.gameMap.getBlockUnsafe(x, y).type = Block::grassBlock;
			}
			else if (y > surface)
			{
				gameData.gameMap.getBlockUnsafe(x, y).type = Block::dirt;
			}
			else
			{
				gameData.gameMap.getBlockUnsafe(x, y).type = Block::air;
			}

			if (y == (int)surface - 1 && (x % 5 == 0 || x % 5 == 1))
			{
				gameData.gameMap.getBlockSafe(x, y)->type = Block::grass;
			}

			gameData.gameMap.getBlockUnsafe(x, y).variant = random(0, 3);


			// wall
			gameData.gameMap.getWallUnsafe(x, y).type = Block::air;
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
	
/* camera movement begin */
	if (IsKeyDown(KEY_A)) { gameData.camera.target.x -= 10.f * deltaTime; }
	if (IsKeyDown(KEY_D)) { gameData.camera.target.x += 10.f * deltaTime; }
	if (IsKeyDown(KEY_W)) { gameData.camera.target.y -= 10.f * deltaTime; }
	if (IsKeyDown(KEY_S)) { gameData.camera.target.y += 10.f * deltaTime; }
/* camera movement end */


/* mouse controls begin */
	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	// bool to avoid destroying walls behind blocks immediately
	static bool bDestroyingBlocks = false;

	// break block
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
	{
		auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

		if (b && b->type != Block::air)
		{
			*b = {};
			bDestroyingBlocks = true;
		}
		else
		{
			auto w = gameData.gameMap.getWallSafe(blockX, blockY);
			if (w && !bDestroyingBlocks)
			{
				*w = {};
			}
		}
		
	}

	// reset destroying blocks bool
	if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON) && bDestroyingBlocks)
	{
		bDestroyingBlocks = false;
	}

	// place block
	if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	{
		// placement of standard blocks
		if (gameData.selectedBlock <= Block::emeraldBlock)
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
					b->variant = random(0, 3);
				}
			}
		}
		// placement of wall blocks
		else
		{
			// check to see if a block has not been placed
			auto block = gameData.gameMap.getBlockSafe(blockX, blockY);
			if (block->type == Block::air)
			{
				auto w = gameData.gameMap.getWallSafe(blockX, blockY);
				if (w && w->type == Block::air)
				{
					w->type = gameData.selectedBlock;
					w->variant = random(0, 3);
				}
			}
		}
	}
/* mouse controls end */


/* draw world begin */
	BeginMode2D(gameData.camera);

	// only draw blocks within the screen dimensions
	Vector2 topLeftView = GetScreenToWorld2D({ 0, 0 }, gameData.camera);
	Vector2 bottomRightView = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, gameData.camera);

	// padding
	int startXView = (int)floorf(topLeftView.x - 1.0f);
	int endXView = (int)ceilf(bottomRightView.x + 1.0f);
	int startYView = (int)floorf(topLeftView.y - 1.0f);
	int endYView = (int)ceilf(bottomRightView.y + 1.0f);


	startXView = Clamp(startXView, 0, gameData.gameMap.w - 1);
	endXView = Clamp(endXView, 0, gameData.gameMap.w - 1);

	startYView = Clamp(startYView, 0, gameData.gameMap.h - 1);
	endYView = Clamp(endYView, 0, gameData.gameMap.h - 1);

	// y outer, x inner for right-down iteration like old tvs
	// draw blocks into world
	for (int y = startYView; y <= endYView; y++)
		for (int x = startXView; x <= endXView; x++)
		{
			
			// walls; drawn first so blocks go in front
			auto& w = gameData.gameMap.getWallUnsafe(x, y);

			if (w.type != Block::air)
			{
				DrawTexturePro(
					assetManager.textures, // texture
					getTextureAtlas(w.type, w.variant, 32, 32), // source
					{ (float)x, (float)y, 1, 1 }, // dest
					{ 0, 0 }, // origin of texture (rotation/scale point) (top-left corner)
					0.f, // rotation
					WHITE // tint
				);
			}

			// blocks
			auto& b = gameData.gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air)
			{
				// draw wood log situationally
				if (b.type == Block::woodLog)
				{
					auto bUp = gameData.gameMap.getBlockSafe(x, y - 1);
					auto bDown = gameData.gameMap.getBlockSafe(x, y + 1);
					auto bRight = gameData.gameMap.getBlockSafe(x + 1, y);
					auto bLeft = gameData.gameMap.getBlockSafe(x - 1, y);

					int treeType = 0;
					if (bDown->type == Block::woodLog || bDown->type == Block::leaves)
					{
						treeType = 0;
						if (bUp->type == Block::leaves)
						{
							treeType = 5;
						}
						else if (bRight->type == Block::leaves && bLeft->type == Block::leaves)
						{
							treeType = 1;
						}
						else if (bRight->type == Block::leaves)
						{
							treeType = 2;
						}
						else if (bLeft->type == Block::leaves)
						{
							treeType = 3;
						}
						else if (bUp->type == Block::air)
						{
							treeType = 6;
						}
					}
					else
					{
						if (bUp->type == Block::air)
						{
							treeType = 7;
						}
						else
						{
							treeType = 4;
						}
					}

					DrawTexturePro(
						assetManager.tree, // texture
						getTextureAtlas(treeType, b.variant, 32, 32), // source
						{ (float)x, (float)y, 1, 1 }, // dest
						{ 0, 0 }, // origin
						0.f, // rotation
						WHITE // tint
					);
				}
				// draw everything else
				else
				{
					DrawTexturePro(
						assetManager.textures, // texture
						getTextureAtlas(b.type, b.variant, 32, 32), // source
						{ (float)x, (float)y, 1, 1 }, // dest
						{ 0, 0 }, // origin of texture (rotation/scale point) (top-left corner)
						0.f, // rotation
						WHITE // tint
					);
				}
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
/* draw world end */


/* simple imgui begin */
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
/* simple imgui end */

	DrawFPS(10, 10);

	return true;
}

void closeGame()
{

}

