#include "structure.h"
#include <asserts.h>

void Structure::create(int width, int height)
{
	blockData.resize(width * height);
	wallData.resize(width * height);

	this->w = width;
	this->h = height;

	for (auto& block : blockData) { block = {}; }
	for (auto& wall : wallData) { wall = {}; }
}

Block& Structure::getBlockUnsafe(int x, int y)
{
	permaAssertCommentDevelopment(blockData.size() == w * h, "Block data not initialized.");

	permaAssertCommentDevelopment(x >= 0 && y >= 0 && x < w && y < h, "getBlockUnsafe out of bounds error.");

	return blockData[y * w + x];
}

Block* Structure::getBlockSafe(int x, int y)
{
	permaAssertCommentDevelopment(blockData.size() == w * h, "Block data not initialized.");

	if (x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

	return &blockData[y * w + x];
}

Block& Structure::getWallUnsafe(int x, int y)
{
	permaAssertCommentDevelopment(wallData.size() == w * h, "Wall data not initialized.");

	permaAssertCommentDevelopment(x >= 0 && y >= 0 && x < w && y < h, "getWallUnsafe out of bounds error.");

	return wallData[y * w + x];
}

Block* Structure::getWallSafe(int x, int y)
{
	permaAssertCommentDevelopment(wallData.size() == w * h, "Wall data not initialized.");

	if (x < 0 || y < 0 || x >= w || y >= h) { return nullptr; }

	return &wallData[y * w + x];
}

