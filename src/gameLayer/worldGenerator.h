#pragma once
#include "gameMap.h"

struct WorldParameters
{
	struct NoiseParameters
	{
		int noiseType = 0;
		float frequency = 0.01f;

		int fractalType = 0;
		int fractalOctaves = 3;
		float fractalLacunarity = 2.f;
		float fractalGain = 0.5f;

		int cellDistFunc = 0;
		int cellReturnType = 0;
		float cellJitter = 1.f;
	};
	int width = 900;
	int height = 400;

	NoiseParameters dirtParams;
	NoiseParameters stoneParams;

	bool bBlendNoise = false;
	int blendNoiseType = 0;
};

void GenerateWorld(GameMap& gameMap, const WorldParameters& params = {}, int seed = 1234);
