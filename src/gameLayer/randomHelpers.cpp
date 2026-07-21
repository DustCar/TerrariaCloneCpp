#include "randomHelpers.h"

int GetRandomInt(std::ranlux24_base& rng, int min, int max)
{
	std::uniform_int_distribution<int>    distr(min, max);
	return distr(rng);
}

float GetRandomFloat(std::ranlux24_base& rng, float min, float max)
{
	std::uniform_real_distribution<float>	distr(min, max);
	return distr(rng);
}

bool GetBoolChance(std::ranlux24_base& rng, float chance)
{
	float roll = GetRandomFloat(rng, 0.f, 1.f);
	return roll <= chance;
}
