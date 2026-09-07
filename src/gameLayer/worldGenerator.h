#pragma once
#include "gameMap.h"

struct WorldParameters
{
	/*
	* Noise types:
	*	- OpenSimplex2, OpenSimplex2S, Cellular, Perlin, ValueCubic, Value
	* 
	* Fractal types:
	*	- None, FBm, Rigid, PingPong
	* 
	* Cell Distance funcitons:
	*	- Euclidean, EuclideanSq, Manhattan, Hybrid
	* 
	* Cell Return types:
	*	- CellValue, Distance, Distance2, Distance2Add, Distance2Sub, Distance2Mul, Distance2Div
	*/
	struct NoiseParameters
	{
		int noiseType = 1;			// noiseType: 1 == OpenSimplex2S
		float frequency = 0.008f;
		float noisePower = 1.7f;

		int fractalType = 1;		// fractalType: 1 == FBm
		int fractalOctaves = 2;
		float fractalLacunarity = 3.f;
		float fractalGain = 0.7f;

		int cellDistFunc = 0;
		int cellReturnType = 0;
		float cellJitter = 1.f;
	};
	int width = 900;
	int height = 400;

	NoiseParameters dirtParams;
	NoiseParameters caveParams;
	NoiseParameters cave2Params;
	NoiseParameters cave3Params;

	NoiseParameters caveBlendParams;

	bool bBlendThirdCaveNoise = true;

	WorldParameters()
	{
		caveParams.noiseType = 1;			// noiseType: 1 == OpenSimplex2S
		caveParams.frequency = 0.05f;
		caveParams.fractalOctaves = 2;
		caveParams.fractalLacunarity = 2.f;
		caveParams.fractalGain = 0.4f;

		cave2Params.frequency = 0.02f;
		cave2Params.fractalOctaves = 1;
		cave2Params.fractalLacunarity = 2.f;
		cave2Params.fractalGain = 0.3f;

		cave3Params.noiseType = 3;			// noiseType: 3 == Perlin
		cave3Params.frequency = 0.04f;
		cave3Params.fractalOctaves = 4;
		cave3Params.fractalLacunarity = 2.f;

		caveBlendParams.noiseType = 3;
		caveBlendParams.frequency = 0.012f;
		caveBlendParams.fractalOctaves = 2;
	}
};

void GenerateWorld(GameMap& gameMap, const WorldParameters& params = {}, int seed = 1234);
