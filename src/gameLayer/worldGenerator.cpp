#include "worldGenerator.h"
#include "randomHelpers.h"
#include <iostream>
#include <FastNoiseLite.h>
#include "helpers.h"

void SetGeneratorParams(FastNoiseLite& generator, int seed, const WorldParameters::NoiseParameters& params);

struct BlockWithChance 
{
	int type;
	float chance;
};

void GenerateWorld(GameMap& gameMap, const WorldParameters& params, int seed)
{
	int w = 900;
	int h = 400;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed++);

	/*std::vector<BlockWithChance> oreBlockChances =
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
	}*/

	// dirt noise
	FastNoiseLite dirtNoiseGenerator;
	SetGeneratorParams(dirtNoiseGenerator, seed, params.dirtParams);

	// stone noise
	FastNoiseLite stoneNoiseGenerator;
	SetGeneratorParams(stoneNoiseGenerator, seed, params.stoneParams);

	// noise data
	std::vector<float> dirtNoise(w);
	std::vector<float> stoneNoise(w);

	// fill data. convert [-1,1] to [0,1] simultaneously
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoiseGenerator.GetNoise((float)i, 0.f) + 1) * 0.5;
		stoneNoise[i] = (stoneNoiseGenerator.GetNoise((float)i, 0.f) + 1) * 0.5;

		// make mountains steep/flat using power
		stoneNoise[i] = std::pow(stoneNoise[i], 0.15f);
	}

	// dirt will be affected by stoneHeight so a -value allows stone to be at most 5 blocks above dirt
	int dirtOffsetStart = -5;
	int dirtOffsetEnd = 30;
	// end - start = 35 layers of dirt

	// stone block range
	int stoneHeightStart = 90;
	int stoneHeightEnd = 180;


	/* Manual World Generation data
	// block range min
	int dirtHeight = 100;
	int stoneHeight = 130;
	int oreOffset = 10;

	// counters
	int dirtCounter = getRandomInt(rng, 5, 35);
	int stoneCounter = getRandomInt(rng, 5, 30);

	// block "slope" direction
	int dirtDir = getRandomInt(rng, -2, 2);
	int stoneDir = getRandomInt(rng, -2, 2);

	*/

	for (int x = 0; x < w; x++)
	{
		/* Manual World Generation
		//// Dirt Blocks 
		dirtCounter--;
		if (dirtCounter <= 0)
		{
			dirtCounter = getRandomInt(rng, 5, 35);
			dirtDir = getRandomInt(rng, -2, 2);
		}
		
		if (dirtDir < 0)
		{
			for (int i = dirtDir; i < 0; i++)
			{
				if (getBoolChance(rng, 0.2f))
				{
					dirtHeight--;
				}
			}
			
		}
		else if (dirtDir > 0)
		{
			for (int i = dirtDir; i > 0; i--)
			{
				if (getBoolChance(rng, 0.25f))
				{
					dirtHeight++;
				}
			}
		}

		//// Stone Blocks 
		stoneCounter--;
		if (stoneCounter <= 0)
		{
			stoneCounter = getRandomInt(rng, 5, 30);
			stoneDir = getRandomInt(rng, -2, 2);
		}

		if (stoneDir < 0)
		{
			for (int i = stoneDir; i < 0; i++)
			{
				if (getBoolChance(rng, 0.2f))
				{
					stoneHeight--;
				}
			}

		}
		else if (stoneDir > 0)
		{
			for (int i = stoneDir; i > 0; i--)
			{
				if (getBoolChance(rng, 0.25f))
				{
					stoneHeight++;
				}
			}
		}

		// cap ranges; dirt has a "max" height, stone has a "min" height
		if (dirtHeight < 75) { dirtHeight = 75; }
		if (stoneHeight > 150) { stoneHeight = 150; }

		*/

		// Lerp heights based on noise
		int stoneHeight = lerp(stoneHeightStart, stoneHeightEnd, stoneNoise[x]);
		int dirtHeight = lerp(dirtOffsetStart, dirtOffsetEnd, dirtNoise[x]);
		dirtHeight = stoneHeight - dirtHeight;

		for (int y = 0; y < h; y++)
		{
			Block b;

			b.variant = getRandomInt(rng, 0, 3);

			if (y > dirtHeight)
			{
				b.type = Block::dirt;
			}
			if (y == dirtHeight)
			{
				b.type = Block::grassBlock;
			}
			
			if (y > stoneHeight)
			{
				b.type = Block::stone;

				/*bool bOre = getBoolChance(rng, 0.1f);
				if (bOre && y > stoneHeight + oreOffset)
				{
					float roll = getRandomFloat(rng, 0.0f, 1.0f);
					auto it = std::lower_bound(oreBlockSum.begin(), oreBlockSum.end(), roll);
					int index = std::distance(oreBlockSum.begin(), it);
					int ore = oreBlockChances[index].type;

					b.type = ore;
				}*/
			}

			gameMap.getBlockUnsafe(x, y) = b;
		}
	}
}


void SetGeneratorParams(FastNoiseLite& generator, int seed, const WorldParameters::NoiseParameters& params)
{
	generator.SetNoiseType((FastNoiseLite::NoiseType)params.noiseType);
	generator.SetSeed(seed++);
	generator.SetFrequency(params.frequency);
	generator.SetFractalType((FastNoiseLite::FractalType)params.fractalType);
	generator.SetFractalOctaves(params.fractalOctaves);
	generator.SetFractalLacunarity(params.fractalLacunarity);
	generator.SetFractalGain(params.fractalGain);

	generator.SetCellularDistanceFunction((FastNoiseLite::CellularDistanceFunction)params.cellDistFunc);
	generator.SetCellularReturnType((FastNoiseLite::CellularReturnType)params.cellReturnType);
	generator.SetCellularJitter(params.cellJitter);
}
