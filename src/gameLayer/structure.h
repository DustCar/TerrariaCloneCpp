#pragma once
#include <vector>
#include <blocks.h>
#include <gameMap.h>
#include <raylib.h>

struct Structure
{
	int w = 0;
	int h = 0;

	std::vector<Block> blockData;
	std::vector<Block> wallData;

	void Create(int width, int height);

	Block& GetBlockUnsafe(int x, int y);
	Block* GetBlockSafe(int x, int y);

	Block& GetWallUnsafe(int x, int y);
	Block* GetWallSafe(int x, int y);

	void CopyFromMap(GameMap& map, Vector2 start, Vector2 end);

	void PasteIntoMap(GameMap& map, Vector2 start);
};