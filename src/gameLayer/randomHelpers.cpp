#include "randomHelpers.h"

int getRandomInt(std::ranlux24_base& rng, int min, int max)
{
	std::uniform_int_distribution<int>    distr(min, max);
	return distr(rng);
}

float getRandomFloat(std::ranlux24_base& rng, float min, float max)
{
	std::uniform_real_distribution<float>	distr(min, max);
	return distr(rng);
}

bool getBoolChance(std::ranlux24_base& rng, float chance)
{
	float roll = getRandomFloat(rng, 0.f, 1.f);
	return roll <= chance;
}
