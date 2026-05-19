#include "worldGenerator.h"
#include "randomHelpers.h"

struct BlockWithChance 
{
	int type;
	float chance;
};

void generateWorld(GameMap& gameMap, int seed)
{
	int w = 900;
	int h = 500;

	gameMap.create(w, h);

	std::vector<BlockWithChance> oreBlockChances =
	{
		{ Block::copper, 0.5f },
		{ Block::iron, 0.3f },
		{ Block::gold, 0.15f },
		{ Block::emerald, 0.05f }
	};

	std::vector<float> oreBlockSum(oreBlockChances.size(), 0.f);

	static float sum = 0;
	for (int i = 0; i < oreBlockChances.size(); i++)
	{
		sum += oreBlockChances[i].chance;
		oreBlockSum[i] = sum;
	}

	int dirtLimit = 50;
	int stoneLimit = 320;

	std::ranlux24_base rng(seed);

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			Block b;

			if (y < h - (dirtLimit + stoneLimit))
			{
				// air; gameMap.create defaults block types 
			}
			else if (y == h - (dirtLimit + stoneLimit))
			{
				b.type = Block::grassBlock;
			}
			else if (y < h - stoneLimit)
			{
				b.type = Block::dirt;
			}
			else
			{
				b.type = Block::stone;

				bool bOre = getBoolChance(rng, 0.1f);
				if (bOre)
				{
					float roll = getRandomFloat(rng, 0.0f, 1.0f);
					auto it = std::lower_bound(oreBlockSum.begin(), oreBlockSum.end(), roll);
					int index = std::distance(oreBlockSum.begin(), it);
					int ore = oreBlockChances[index].type;

					b.type = ore;
				}
			}

			gameMap.getBlockUnsafe(x, y) = b;
		}
	}
}
