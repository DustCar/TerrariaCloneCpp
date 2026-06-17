#include "worldGenerator.h"
#include "randomHelpers.h"
#include <iostream>
#include <FastNoiseLite.h>
#include "helpers.h"

void SetGeneratorParams(FastNoiseLite& generator, int seed, const WorldParameters::NoiseParameters& params);

void GenerateWorld(GameMap& gameMap, const WorldParameters& params, int seed)
{
	int w = 900;
	int h = 400;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed++);

	// dirt noise
	FastNoiseLite dirtNoiseGenerator;
	SetGeneratorParams(dirtNoiseGenerator, seed, params.dirtParams);

	// noise data
	std::vector<float> dirtNoise(w);

	// fill data. convert [-1,1] to [0,1] simultaneously
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoiseGenerator.GetNoise((float)i, 0.f) + 1) * 0.5;

		// make mountains steep/flat using power
		dirtNoise[i] = std::pow(dirtNoise[i], params.dirtParams.noisePower);
	}

	// dirt will be affected by stoneHeight so a -value allows stone to be at most 5 blocks above dirt
	int dirtOffsetStart = -7;
	int dirtOffsetEnd = 35;
	// end - start = 42 layers of dirt

	
	/* Manual World Generation data */
	// stone block range
	int stoneHeight = 120;

	// counters
	int stoneDirectionTimer = getRandomInt(rng, 10, 30);

	// block "slope" direction
	int stoneDir = getRandomInt(rng, -2, 2);


	for (int x = 0; x < w; x++)
	{
		/* Manual World Generation */
		// Stone Blocks 
		stoneDirectionTimer--;
		if (stoneDirectionTimer <= 0)
		{
			stoneDirectionTimer = getRandomInt(rng, 10, 30);
			stoneDir = getRandomInt(rng, -2, 2);
		}

		if (stoneDir < 0)
		{
			for (int i = stoneDir; i < 0; i++)
			{
				if (getBoolChance(rng, 0.25f))
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

		// Lerp heights based on noise
		int dirtHeight = lerp(dirtOffsetStart, dirtOffsetEnd, dirtNoise[x]);
		dirtHeight = stoneHeight - dirtHeight;

		// cap ranges; 
		if (stoneHeight < 75) { stoneHeight = 75; }
		if (stoneHeight > 150) { stoneHeight = 150; }

		
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
