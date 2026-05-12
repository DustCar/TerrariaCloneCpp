#pragma once
#include <vector>
#include <blocks.h>

struct GameMap
{
	int w = 0;
	int h = 0;

	std::vector<Block> mapData;
	std::vector<Block> wallData;

	void create(int width, int height);

	Block& getBlockUnsafe(int x, int y);
	Block* getBlockSafe(int x, int y);

	Block& getWallUnsafe(int x, int y);
	Block* getWallSafe(int x, int y);
};

