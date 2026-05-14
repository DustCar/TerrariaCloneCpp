#pragma once

#include <raylib.h>
#include <random>

// obtain a certain texture from an atlas
Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY);

// standard uniform rand function
template<typename T>
T random(T range_from, T range_to) {
    std::random_device                  rand_dev;
    std::ranlux24_base                  generator(rand_dev());
    std::uniform_int_distribution<T>    distr(range_from, range_to);
    return distr(generator);
}

template<typename T>
T randomWithSeed(T seed, T range_from, T range_to) {
    std::ranlux24_base                  generator(seed);
    std::uniform_int_distribution<T>    distr(range_from, range_to);
    return distr(generator);
}
