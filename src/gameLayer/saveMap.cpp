#include "saveMap.h"
#include <asserts.h>

// keep a copy of a version of the block structure to keep track of different saves
struct BlockSaveRepresentation1
{
	std::uint16_t type = 0;
	std::uint8_t variant = 0;

	Block toBlock()
	{
		Block b;
		b.type = type;
		b.variant = variant;
		return b;
	}
};

struct BlockSaveRepresentation2
{
	std::uint16_t type = 0;
	std::uint8_t variant = 0;
	std::uint8_t durability = 1;

	Block toBlock()
	{
		Block b;
		b.type = type;
		b.variant = variant;
		b.durability = durability;
		return b;
	}
};

const int VERSION = 2;

// converter to transform the block to a specific save version (new or old)
BlockSaveRepresentation2 toBlockSaveRepresentation(Block b)
{
	BlockSaveRepresentation2 saveRep;
	saveRep.type = b.type;
	saveRep.variant = b.variant;
	saveRep.durability = b.durability;
	return saveRep;
}

bool saveBlockDataToFile(const std::vector<Block>& blocks, const std::vector<Block>& walls, int width, int height, const char* filename)
{
	// open output file stream in binary mode
	std::ofstream f(filename, std::ios::binary);

	// check file
	if (!f.is_open()) { return false; }

	// assert and check blocks size
	permaAssertDevelopment(walls.size() == width * height);
	permaAssertDevelopment(walls.size() != 0);
	if (walls.size() != width * height) { return false; }
	if (walls.size() == 0) { return false; }

	permaAssertDevelopment(blocks.size() == width * height);
	permaAssertDevelopment(blocks.size() != 0);
	if (blocks.size() != width * height) { return false; }
	if (blocks.size() == 0) { return false; }
	
	// write the version number first to distinguish save system versions
	f.write(reinterpret_cast<const char*>(&VERSION), sizeof(VERSION));
	// write w and h to file
	if (!f.write(reinterpret_cast<const char*>(&width), sizeof(width)) || !f.write(reinterpret_cast<const char*>(&height), sizeof(height)))
	{
		return false;
	}

	// write blocks data to file
	for (int i = 0; i < blocks.size(); i++)
	{
		auto w = toBlockSaveRepresentation(walls[i]);
		auto b = toBlockSaveRepresentation(blocks[i]);

		if (!f.write(reinterpret_cast<const char*>(&w), sizeof(w)))
		{
			return false;
		}
		if (!f.write(reinterpret_cast<const char*>(&b), sizeof(b)))
		{
			return false;
		}
	}

	f.close();
	return true;
}

// returns a vector of blocks and dimensions
bool loadBlockDataFromFile(std::vector<Block>& blocks, std::vector<Block>& walls, int& width, int& height, const char* filename)
{
	// clear data to start clean
	auto clearData = [&]()
	{
		walls.clear();
		blocks.clear();
		width = 0;
		height = 0;
	};
	clearData();

	// open input file stream
	std::ifstream f(filename, std::ios::binary);

	if (!f.is_open()) { return false; }
	
	int readVersion = 0;

	// read dimensions
	f.read(reinterpret_cast<char*>(&readVersion), sizeof(readVersion));
	f.read(reinterpret_cast<char*>(&width), sizeof(width));
	f.read(reinterpret_cast<char*>(&height), sizeof(height));

	// check if file can't be read or if either dimension is 0 or smaller
	if (!f || width <= 0 || height <= 0)
	{
		clearData();
		return false;
	}

	// data is probably corrupt
	if (width > 10000) { f.close(); return false; }
	if (height > 10000) { f.close(); return false; }

	// read block data
	// TODO: refactor to a switch statement where saving is different based on version
	switch (readVersion)
	{
		case 1:
		{
			size_t blockCount = width * height;
			walls.resize(blockCount);
			blocks.resize(blockCount);

			for (int i = 0; i < blockCount; i++)
			{
				BlockSaveRepresentation1 readWall;
				BlockSaveRepresentation1 readBlock;

				f.read(reinterpret_cast<char*>(&readWall), sizeof(readWall));
				f.read(reinterpret_cast<char*>(&readBlock), sizeof(readBlock));

				// clear and return false if file can't be read
				if (!f)
				{
					clearData();
					return false;
				}

				// convert to current block version
				walls[i] = readWall.toBlock();
				blocks[i] = readBlock.toBlock();
			}

			break;
		}

		case 2:
		{
			size_t blockCount = width * height;
			walls.resize(blockCount);
			blocks.resize(blockCount);

			for (int i = 0; i < blockCount; i++)
			{
				BlockSaveRepresentation2 readWall;
				BlockSaveRepresentation2 readBlock;

				f.read(reinterpret_cast<char*>(&readWall), sizeof(readWall));
				f.read(reinterpret_cast<char*>(&readBlock), sizeof(readBlock));

				// clear and return false if file can't be read
				if (!f)
				{
					clearData();
					return false;
				}

				// convert to current block version
				walls[i] = readWall.toBlock();
				blocks[i] = readBlock.toBlock();
			}

			break;
		}
			
		default: // no available version selected
		{
			width = 0;
			height = 0;
			return false;
			break;
		}
	}

	// remove any invalid blocks if any
	for (int i = 0; i < walls.size(); i++)
	{
		walls[i].Sanitize();
	}
	for (int i = 0; i < blocks.size(); i++)
	{
		blocks[i].Sanitize();
	}

	f.close();
	return true;
}
