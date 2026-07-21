#pragma once
#include <random>

// returns an int in [min, max]
int GetRandomInt(std::ranlux24_base& rng, int min, int max);

// returns a float in [min, max)
float GetRandomFloat(std::ranlux24_base& rng, float min, float max);

// returns bool based on chance
bool GetBoolChance(std::ranlux24_base& rng, float chance);
