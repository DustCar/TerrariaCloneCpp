#include "saveMap.h"
#include <asserts.h>

bool saveBlockDataToFile(std::vector<Block> blocks, int width, int height, const char* filename)
{
	// open output file stream in binary mode
	std::ofstream f(filename, std::ios::binary);

	// check file
	if (!f.is_open()) { return false; }

	// assert and check blocks size
	permaAssertDevelopment(blocks.size() == width * height);
	permaAssertDevelopment(blocks.size() != 0);
	if (blocks.size() != width * height) { return false; }
	if (blocks.size() == 0) { return false; }

	// write w and h to file
	if (!f.write((const char*)&width, sizeof(width)) || !f.write((const char*)&height, sizeof(height)))
	{
		return false;
	}

	// write blocks data to file
	if (!f.write((const char*)blocks.data(), sizeof(Block) * blocks.size()))
	{
		return false;
	}

	f.close();
	return true;
}

// returns a vector of blocks and dimensions
bool loadBlockDataFromFile(std::vector<Block>& blocks, int& width, int& height, const char* filename)
{
	// clear data to start clean
	blocks.clear();
	width = 0;
	height = 0;

	// open input file stream
	std::ifstream f(filename, std::ios::binary);

	if (!f.is_open()) { return false; }

	// read dimensions
	f.read((char*)&width, sizeof(width));
	f.read((char*)&height, sizeof(height));

	// check if file can't be read or if either dimension is 0 or smaller
	if (!f || width <= 0 || height <= 0)
	{
		return false;
	}

	// data is probably corrupt
	if (width > 10000) { f.close(); return false; }
	if (height > 10000) { f.close(); return false; }

	// read block data
	size_t blockCount = width * height;
	blocks.resize(blockCount);

	f.read((char*)blocks.data(), sizeof(Block) * blockCount);

	// if file can't be read again, then clear data and return false
	if (!f)
	{
		blocks.clear();
		width = 0;
		height = 0;
		return false;
	}

	// remove any invalid blocks if any
	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i].sanitize();
	}

	f.close();
	return true;
}
