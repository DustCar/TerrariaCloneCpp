#include "gameMap.h"
#include <asserts.h>

void GameMap::Create(int width, int height)
{
	// reset data
	*this = {};
	mapData.resize(width * height);
	wallData.resize(width * height);

	this->w = width;
	this->h = height;

	for (auto& block : mapData) { block = {}; }	// reset all block data
	for (auto& wall : wallData) { wall = {}; }
}

Block& GameMap::GetBlockUnsafe(int x, int y)
{
	permaAssertCommentDevelopment(mapData.size() == w * h, "Map data not initialized.");

	permaAssertCommentDevelopment(x >= 0 && y >= 0 && x < w && y < h, "getBlockUnsafe out of bounds error.");

	return mapData[y * w + x];

}

Block* GameMap::GetBlockSafe(int x, int y)
{
	permaAssertCommentDevelopment(mapData.size() == w * h, "Map data not initialized.");

	if (x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

	return &mapData[y * w + x];
}

Block& GameMap::GetWallUnsafe(int x, int y)
{
	permaAssertCommentDevelopment(wallData.size() == w * h, "Wall data not initialized.");

	permaAssertCommentDevelopment(x >= 0 && y >= 0 && x < w && y < h, "getWallUnsafe out of bounds error.");

	return wallData[y * w + x];
}

Block* GameMap::GetWallSafe(int x, int y)
{
	permaAssertCommentDevelopment(wallData.size() == w * h, "Wall data not initialized.");

	if (x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

	return &wallData[y * w + x];
}
