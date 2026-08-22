#pragma once

#include <raylib.h>
#include <string>
#include <vector>

// obtain a certain texture from an atlas
Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY);

float lerp(float a, float b, float t);

// implement read entire file func
bool readEntireFile(const char* fileName, std::string& data);

bool readEntireFile(const char* fileName, std::vector<char>& data);

// implement write entire file func
bool writeEntireFile(const char* fileName, const void* data, size_t size);

