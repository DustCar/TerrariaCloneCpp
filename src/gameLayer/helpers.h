#pragma once

#include <raylib.h>

// obtain a certain texture from an atlas
Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY);

float lerp(float a, float b, float t);
