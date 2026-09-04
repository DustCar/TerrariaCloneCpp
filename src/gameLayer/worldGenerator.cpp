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

	gameMap.Create(w, h);

	std::ranlux24_base rng(seed++);

	// dirt noise
	FastNoiseLite dirtNoiseGenerator;
	SetGeneratorParams(dirtNoiseGenerator, seed, params.dirtParams);

	// cave noise
	FastNoiseLite caveNoiseGenerator;
	SetGeneratorParams(caveNoiseGenerator, seed, params.caveParams);
	FastNoiseLite caveNoiseGenerator2;
	SetGeneratorParams(caveNoiseGenerator2, seed, params.cave2Params);
	FastNoiseLite caveNoiseGenerator3;
	SetGeneratorParams(caveNoiseGenerator3, seed, params.cave3Params);
	FastNoiseLite caveNoiseGeneratorBlend;
	SetGeneratorParams(caveNoiseGeneratorBlend, seed, params.caveBlendParams);

	// noise data
	std::vector<float> dirtNoise(w);
	std::vector<float> caveNoise(w * h);
	std::vector<float> caveNoise2(w * h);
	std::vector<float> caveNoise3(w * h);
	std::vector<float> caveNoiseBlend(w * h);

	// fill data. convert [-1,1] to [0,1] simultaneously
	for (int i = 0; i < w; i++)
	{
		dirtNoise[i] = (dirtNoiseGenerator.GetNoise((float)i, 0.f) + 1) * 0.5f;

		// make mountains steep/flat using power
		dirtNoise[i] = std::pow(dirtNoise[i], params.dirtParams.noisePower);
	}

	// helper to fill noise vectors
	auto fillNoiseDataFull = [&](std::vector<float>& noise, const FastNoiseLite& noiseGenerator)
	{
		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
				noise[j * w + i] = (noiseGenerator.GetNoise((float)i, (float)j) + 1) * 0.5f;
		}
	};

	fillNoiseDataFull(caveNoise, caveNoiseGenerator);
	fillNoiseDataFull(caveNoise2, caveNoiseGenerator2);
	fillNoiseDataFull(caveNoise3, caveNoiseGenerator3);
	fillNoiseDataFull(caveNoiseBlend, caveNoiseGeneratorBlend);

	// helper to get cave noise
	auto getCaveNoise = [&](const std::vector<float>& noise, int x, int y)
	{
		return noise[y * w + x];
	};

	
	/* Manual World Generation data */
	// dirt layer only cares about normal blocks, no need to look at wall blocks
	auto generateDirtLayer = [&]()
	{
		int dirtBaseMin = 55;
		int dirtBaseMax = 85;

		for (int x = 0; x < w; x++)
		{
			// lerp height from noise
			int dirtHeight = lerp(dirtBaseMin, dirtBaseMax, dirtNoise[x]);

			for (int y = 0; y < h; y++)
			{
				Block b = gameMap.GetBlockUnsafe(x, y);

				b.variant = getRandomInt(rng, 0, 3);

				if (y == dirtHeight)
				{
					b.type = Block::grassBlock;
				}
				if (y > dirtHeight)
				{
					b.type = Block::dirt;
				}
				
				gameMap.GetBlockUnsafe(x, y) = b;
			}
		}

	};

	int stoneHeight = 90;

	// Stone layer generation, same thing, no wall blocks
	auto generateStoneLayer = [&]()
	{
		std::cout << "Stone generated\n";

		// counters
		int stoneDirectionTimer = getRandomInt(rng, 10, 30);

		// block "slope" direction
		int stoneDir = getRandomInt(rng, -1, 1);

		for (int x = 0; x < w; x++)
		{
			// Stone Blocks 
			stoneDirectionTimer--;
			if (stoneDirectionTimer <= 0)
			{
				stoneDirectionTimer = getRandomInt(rng, 10, 30);
				stoneDir = getRandomInt(rng, -1, 1);
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

			// cap ranges; 
			if (stoneHeight < 75) { stoneHeight = 75; }
			if (stoneHeight > 120) { stoneHeight = 120; }

			for (int y = 0; y < h; y++)
			{
				Block b = gameMap.GetBlockUnsafe(x, y);

				b.variant = getRandomInt(rng, 0, 3);

				if (y >= stoneHeight)
				{
					b.type = Block::stone;
				}

				gameMap.GetBlockUnsafe(x, y) = b;
			}
		}
	};
	
	generateDirtLayer();
	generateStoneLayer();

	// desert biome generation
	const int desertEndPercent = (int)(w * 0.7f);
	int desertBiomeStart = getRandomInt(rng, 50, desertEndPercent - 50);

	// used as min length and for extra length for desert end
	int desertEndPaddingHalf = (int)((w - desertEndPercent + 50) * 0.5f);
	int desertBiomeEnd = desertBiomeStart + desertEndPaddingHalf + getRandomInt(rng, 0, desertEndPaddingHalf);
	if (desertBiomeEnd > w) { desertBiomeEnd = w; }

	// actual mid point value on map
	int desertBiomeMid = (desertBiomeStart + desertBiomeEnd) * 0.5f;
	// size of desert half
	int desertHalfWidth = (desertBiomeEnd - desertBiomeStart) * 0.5f;

	for (int x = 0; x < w; x++)
	{
		bool bInDesert = (x >= desertBiomeStart && x <= desertBiomeEnd);

		/* Manual World Generation */
		

		for (int y = 0; y < h; y++)
		{
			auto b = gameMap.GetBlockUnsafe(x,y);

			b.variant = getRandomInt(rng, 0, 3);
			
			if (y >= stoneHeight)
			{
				if (bInDesert)
				{
					b.type = Block::sandStone;
				}
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

			// Cave generation block
			float screenBlend = 1.f - (1.f - getCaveNoise(caveNoise, x, y)) * (1.f - getCaveNoise(caveNoise2, x, y));
			if (params.bBlendThirdCaveNoise)
			{

				float blendResult = lerp(screenBlend, getCaveNoise(caveNoise3, x, y), getCaveNoise(caveNoiseBlend, x, y));
				if (blendResult < 0.5f && blendResult > 0.35f && y > stoneHeight + 2)
				{
					b.type = Block::air;
				}
			}
			else
			{
				if (screenBlend < 0.5f && screenBlend > 0.35f && y > stoneHeight + 2)
				{
					b.type = Block::air;
				}
			}
			

			gameMap.GetBlockUnsafe(x, y) = b;
		}
	}

	auto SpawnWorm = [&](float startX, float startY, int wormMin, int wormMax, float maxRadius)
	{
		// direction worm moves towards
		float dirX = getRandomFloat(rng, -1.f, 1.f);
		float dirY = getRandomFloat(rng, -1.f, 1.f);

		// bias worm movement based on if it spawns near the edges
		if (startX < 20)
		{
			dirX = getRandomFloat(rng, 0.8f, 1.f);
		}
		else if (startX > w - 20)
		{
			dirX = getRandomFloat(rng, -1.f, -0.8f);
		}

		if (startY < stoneHeight)
		{
			dirY = getRandomFloat(rng, 0.8f, 1.f);
		}
		else if (startY > h - 20)
		{
			dirY = getRandomFloat(rng, -1.f, -0.8f);
		}

		int wormLength = getRandomInt(rng, wormMin, wormMax);
		float radius = 3.5f;

		int changeDirectionTime = getRandomInt(rng, 5, 20);

		// draw worm
		for (int j = 0; j < wormLength; j++)
		{
			// draw segment
			int radiusInt = std::ceil(radius); // ceil to compensate for the extra .5, creating a fuller circle
			for (int ox = -radiusInt; ox <= radiusInt; ox++)
			{
				for (int oy = -radiusInt; oy <= radiusInt; oy++)
				{

					float distSqrd = ox * ox + oy * oy;
					if (distSqrd <= radius * radius)
					{
						float dx = startX + ox;
						float dy = startY + oy;

						auto b = gameMap.GetBlockSafe(dx, dy);
						if (b)
						{
							b->type = Block::air;
						}
					}
				}
			}

			changeDirectionTime--;
			if (changeDirectionTime <= 0)
			{
				changeDirectionTime = getRandomInt(rng, 5, 20);

				float keepDirAmount = 0.65f;

				if (getBoolChance(rng, 0.75f))
				{
					// keeps a large portion of the original direction, with minor adjustments
					dirX = dirX * keepDirAmount + (getRandomFloat(rng, -1.f, 1.f) * (1.f - keepDirAmount));
					dirY = dirY * keepDirAmount + (getRandomFloat(rng, -1.f, 1.f) * (1.f - keepDirAmount));
				}
				else
				{
					// opposite, keeps a small portion and has major adjustments
					dirX = dirX * (1.f - keepDirAmount) + (getRandomFloat(rng, -1.f, 1.f) * keepDirAmount);
					dirY = dirY * (1.f - keepDirAmount) + (getRandomFloat(rng, -1.f, 1.f) * keepDirAmount);
				}

			}

			startX += dirX * 1.5f;
			startY += dirY * 1.5f;

			radius += (getRandomFloat(rng, -0.2f, 0.2f));
			radius = std::clamp(radius, 2.5f, maxRadius);
		}
	};

	// perlin worms without the perlin noise
	int nWorms = 12;
	for (int i = 0; i < nWorms; i++)
	{
		// separate the worms "uniformly" to avoid too much clumping
		float segmentLength = w / (float)nWorms;
		SpawnWorm(getRandomFloat(rng, segmentLength * i, segmentLength * (i + 1)), getRandomFloat(rng, stoneHeight, h - 15.f), 75, 200, 5.8f);
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
