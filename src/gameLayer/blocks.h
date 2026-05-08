#pragma once
#include <cstdint>

struct Block
{
	enum
	{
		air = 0,
		dirt,

		BLOCKS_COUNT,

	};

	// block type. at most 65535 block types.
	std::uint16_t type = 0;


};