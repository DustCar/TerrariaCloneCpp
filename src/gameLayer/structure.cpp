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

void Structure::copyFromMap(GameMap& map, Vector2 start, Vector2 end)
{
	// multiple checks to constrain start and end coords
	if (start.x > map.w) { start.x = map.w - 1; }
	if (end.x > map.w) { end.x = map.w - 1; }

	if (start.y > map.h) { start.y = map.h - 1; }
	if (end.y > map.h) { end.y = map.h - 1; }

	if (start.x < 0) { start.x = 0; }
	if (start.y < 0) { start.y = 0; }

	if (end.x < 0) { end.x = 0; }
	if (end.y < 0) { end.y = 0; }

	if (start.x > end.x) { std::swap(start.x, end.x); }
	if (start.y > end.y) { std::swap(start.y, end.y); }

	// calculate size of selection
	Vector2 size = Vector2{ end.x - start.x + 1, end.y - start.y + 1 };

	// discard if selecting anything bigger than map
	if (size.x > map.w) { return; }
	if (size.y > map.h) { return; }

	// initialize the dimensions and arrays
	create(size.x, size.y);

	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			getWallUnsafe(x, y) = map.getWallUnsafe(x + start.x, y + start.y);
			getBlockUnsafe(x, y) = map.getBlockUnsafe(x + start.x, y + start.y);
		}
	}
}

void Structure::pasteIntoMap(GameMap& map, Vector2 start)
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			auto w = getWallSafe(x, y);
			if (w)
			{
				*w = getWallUnsafe(x + start.x, y + start.y);
			}

			auto b = getBlockSafe(x, y);

			if (b)
			{
				*b = getBlockUnsafe(x + start.x, y + start.y);
			}
		}
	}
}

