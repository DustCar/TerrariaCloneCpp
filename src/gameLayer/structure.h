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

	void create(int width, int height);

	Block& getBlockUnsafe(int x, int y);
	Block* getBlockSafe(int x, int y);

	Block& getWallUnsafe(int x, int y);
	Block* getWallSafe(int x, int y);

	void copyFromMap(GameMap& map, Vector2 start, Vector2 end);

	void pasteIntoMap(GameMap& map, Vector2 start);
};