#pragma once
#include <cstdint>

struct Block
{
	enum
	{
		// standard blocks
		air = 0,
		dirt,
		grassBlock,
		stone,
		grass,
		sand,
		sandRuby,
		sandStone,
		woodPlank,
		stoneBricks,
		clay,
		woodLog,
		leaves,
		copper,
		iron,
		gold,
		copperBlock,
		ironBlock,
		goldBlock,
		bricks,
		snow,
		ice,
		rubyBlock,
		platform,
		workBench,
		glass,
		furnace,
		painting,
		sappling,
		snowBlueRuby,
		blueRubyBlock,
		door,
		jar,
		table,
		wordrobe,
		bookShelf,
		snowBricks,
		iceTable,
		iceWordrobe,
		iceBookShelf,
		icePlatform,
		sandTable,
		sandWordrobe,
		sandBookShelf,
		sandPlatform,
		woodenChest,
		iceChest,
		sandChest,
		boneChest,
		boneBricks,
		boneBench,
		boneWordrobe,
		boneBookShelf,
		bonePlatform,
		emerald,
		emeraldBlock,

		// wall blocks
		dirtWall,
		stoneWall,
		woodWall,
		sandStoneWall,
		brickWall,
		glassWall,
		copperBlockWall,
		silverBlockWall,
		goldBlockWall,
		snowWall,
		sandWall,
		stoneBrickWall,
		rubyBlockWall,
		hieroglyphWall,
		blueRubyBlockWall,
		plankedWall,
		snowBrickWall,
		boneBrickWall,
		emeraldBlockWall,

		BLOCKS_COUNT,

	};

	// block type. at most 65535 block types.
	std::uint16_t type = 0;
	
	// style of the block (row in texture atlas)
	std::uint8_t variant = 0;

	std::uint8_t durability = 1;

	// makes sure that a block is valid. typically a placeholder would be used here
	void Sanitize()
	{
		if (type >= BLOCKS_COUNT)
		{
			type = 0;
		}
	}

};