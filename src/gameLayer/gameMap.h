#pragma once
#include <vector>
#include <blocks.h>

struct GameMap
{
	int w = 0;
	int h = 0;

	std::vector<Block> mapData;
	std::vector<Block> wallData;

	void Create(int width, int height);

	Block& GetBlockUnsafe(int x, int y);
	Block* GetBlockSafe(int x, int y);

	Block& GetWallUnsafe(int x, int y);
	Block* GetWallSafe(int x, int y);
};

