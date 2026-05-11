#include "gameMap.h"
#include <asserts.h>

void GameMap::create(int width, int height)
{
	// reset data
	*this = {};
	mapData.resize(width * height);

	this->w = width;
	this->h = height;

	for (auto& block : mapData) { block = {}; }	// reset all block data
}

Block& GameMap::getBlockUnsafe(int x, int y)
{
	permaAssertCommentDevelopment(mapData.size() == w * h, "Map data not initialized");

	permaAssertCommentDevelopment(x >= 0 && y >= 0 && x < w && y < h, "getBlockUnsafe out of bounds error");

	return mapData[y * w + x];

}

Block* GameMap::getBlockSafe(int x, int y)
{
	permaAssertCommentDevelopment(mapData.size() == w * h, "Map data not initialized");

	if (x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

	return &mapData[y * w + x];
}
