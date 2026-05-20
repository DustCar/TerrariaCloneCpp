#include "worldGenerator.h"
#include "randomHelpers.h"
#include <iostream>

struct BlockWithChance 
{
	int type;
	float chance;
};

void generateWorld(GameMap& gameMap, int seed)
{
	int w = 900;
	int h = 350;

	gameMap.create(w, h);

	std::vector<BlockWithChance> oreBlockChances =
	{
		{ Block::copper, 0.5f },
		{ Block::iron, 0.3f },
		{ Block::gold, 0.15f },
		{ Block::emerald, 0.05f }
	};

	std::vector<float> oreBlockSum(oreBlockChances.size(), 0.f);

	float sum = 0;
	for (int i = 0; i < oreBlockChances.size(); i++)
	{
		sum += oreBlockChances[i].chance;
		oreBlockSum[i] = sum;
	}

	int dirtLimit = 130;
	int stoneLimit = 180;

	std::ranlux24_base rng(seed);

	int minDirt = 90, maxDirt = 159;
	int minStone = 160, maxStone = 225;

	int targetDirt = getRandomInt(rng, minDirt, maxDirt);
	int targetStone = getRandomInt(rng, minStone, maxStone);

	// blocks from targeted dirt/stone height to switch target up
	int thresholdDirt = 33; 
	int thresholdStone = 32;

	for (int x = 0; x < w; x++)
	{
		dirtLimit += (int)((targetDirt - dirtLimit) * 0.04f);
		stoneLimit += (int)((targetStone - stoneLimit) * 0.04f);

		if (abs(targetDirt - dirtLimit) < thresholdDirt) { targetDirt = getRandomInt(rng, minDirt, maxDirt); }
		if (abs(targetStone - stoneLimit) < thresholdStone) { targetStone = getRandomInt(rng, minStone, maxStone); }

		for (int y = 0; y < h; y++)
		{
			Block b;

			if (y > dirtLimit)
			{
				b.type = Block::dirt;
			}
			if (y == dirtLimit)
			{
				b.type = Block::grassBlock;
			}
			
			if (y > stoneLimit)
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
