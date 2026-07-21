#include "worldGenerator.h"
#include "randomHelpers.h"
#include <iostream>
#include <FastNoiseLite.h>
#include "helpers.h"

void SetGeneratorParams(FastNoiseLite& generator, int seed, const WorldParameters::NoiseParameters& params);

void GenerateWorld(GameMap& gameMap, const WorldParameters& params, int seed)
{
	int w = params.width;
	int h = params.height;

	gameMap.create(w, h);

	std::ranlux24_base rng(seed++);

	// dirt noise
	FastNoiseLite dirtNoiseGenerator;
	SetGeneratorParams(dirtNoiseGenerator, seed, params.dirtParams);

	// cave noise
	FastNoiseLite caveNoiseGenerator;
	SetGeneratorParams(caveNoiseGenerator, seed, params.caveParams);

	FastNoiseLite caveNoiseGenerator2;
	SetGeneratorParams(caveNoiseGenerator2, seed, params.cave2Params);

	// noise data
	std::vector<float> dirtNoise(w);
	std::vector<float> caveNoise(w * h);
	std::vector<float> caveNoise2(w * h);

	// fill data. convert [-1,1] to [0,1] simultaneously
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoiseGenerator.GetNoise((float)i, 0.f) + 1) * 0.5f;

		// make mountains steep/flat using power
		dirtNoise[i] = std::pow(dirtNoise[i], params.dirtParams.noisePower);
	}

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			caveNoise[j * w + i] = (caveNoiseGenerator.GetNoise((float)i, (float)j) + 1) * 0.5f;
			caveNoise2[j * w + i] = (caveNoiseGenerator2.GetNoise((float)i, (float)j) + 1) * 0.5f;
		}
	}

	// helper to get cave noise
	auto GetCaveNoise = [&](const std::vector<float>& data, int x, int y)
	{
		return data[y * w + x];
	};

	// dirt will be affected by stoneHeight so a -value allows stone to be at most 5 blocks above dirt
	int dirtOffsetStart = -7;
	int dirtOffsetEnd = 43;
	// end - start = 57 layers of dirt

	
	/* Manual World Generation data */

	// desert biome generation
	const int desertEndPercent = (int)(w * 0.7f);
	int desertBiomeStart = GetRandomInt(rng, 50, desertEndPercent - 50);

	// used as min length and for extra length for desert end
	int desertEndPaddingHalf = (int)((w - desertEndPercent + 50) * 0.5f);
	int desertBiomeEnd = desertBiomeStart + desertEndPaddingHalf + GetRandomInt(rng, 0, desertEndPaddingHalf);
	if (desertBiomeEnd > w) { desertBiomeEnd = w; }

	// actual mid point value on map
	int desertBiomeMid = (desertBiomeStart + desertBiomeEnd) * 0.5f;
	// size of desert half
	int desertHalfWidth = (desertBiomeEnd - desertBiomeStart) * 0.5f;

	// stone block range
	int stoneHeight = 90;

	// counters
	int stoneDirectionTimer = GetRandomInt(rng, 10, 30);

	// block "slope" direction
	int stoneDir = GetRandomInt(rng, -1, 1);


	for (int x = 0; x < w; x++)
	{
		bool bInDesert = (x >= desertBiomeStart && x <= desertBiomeEnd);

		/* Manual World Generation */
		// Stone Blocks 
		stoneDirectionTimer--;
		if (stoneDirectionTimer <= 0)
		{
			stoneDirectionTimer = GetRandomInt(rng, 10, 30);
			stoneDir = GetRandomInt(rng, -1, 1);
		}

		if (stoneDir < 0)
		{
			for (int i = stoneDir; i < 0; i++)
			{
				if (GetBoolChance(rng, 0.25f))
				{
					stoneHeight--;
				}
			}

		}
		else if (stoneDir > 0)
		{
			for (int i = stoneDir; i > 0; i--)
			{
				if (GetBoolChance(rng, 0.25f))
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
		if (stoneHeight > 120) { stoneHeight = 120; }

		int dirtType = Block::dirt;
		int grassType = Block::grassBlock;
		int stoneType = Block::stone;

		if (bInDesert)
		{
			dirtType = Block::sand;
			grassType = Block::sand;
			stoneType = Block::sandStone;
		}

		for (int y = 0; y < h; y++)
		{
			Block b;

			b.variant = GetRandomInt(rng, 0, 3);

			if (y > dirtHeight)
			{
				b.type = dirtType;
			}

			if (y == dirtHeight)
			{
				b.type = grassType;
			}
			
			if (y > stoneHeight)
			{
				b.type = stoneType;

			}

			if (bInDesert)
			{
				// use the desert mid and width to create a triangle shape by using a desert threshold
				int desertDistanceFromMid = std::abs(x - desertBiomeMid);
				// 0 at the edge, 1 at the middle
				float desertMidDistanceRatio = 1 - desertDistanceFromMid / (float)desertHalfWidth;

				desertMidDistanceRatio = std::clamp(desertMidDistanceRatio, 0.f, 1.f);
				// power to affect shape
				desertMidDistanceRatio = powf(desertMidDistanceRatio, 1.15f);

				// when regular stone starts
				int desertStoneThreshold = stoneHeight + 5;
				// how deep the shape goes
				int desertStoneDepth = stoneHeight - 15;

				int desertShapeY = desertStoneThreshold + desertMidDistanceRatio * desertStoneDepth;

				if (y > desertShapeY)
				{
					b.type = Block::stone;
				}
			}

			float screenBlend = 1.f - (1.f - GetCaveNoise(caveNoise, x, y)) * (1.f - GetCaveNoise(caveNoise2, x, y));
			if (screenBlend < 0.5f && screenBlend > 0.2f && y > stoneHeight + 2)
			{
				b.type = Block::air;
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
