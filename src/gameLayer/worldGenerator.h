#pragma once
#include "gameMap.h"

struct WorldParameters
{
	/*
	* Noise types:
	*	- OpenSimplex2, OpenSimplex2S, Cellular, Perlin, ValueCubic, Value
	* 
	* Fractal types:
	*	- None, FBm, Rigid
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

};

void GenerateWorld(GameMap& gameMap, const WorldParameters& params = {}, int seed = 1234);
