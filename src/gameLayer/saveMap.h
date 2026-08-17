#pragma once
#include <vector>
#include <blocks.h>
#include <fstream>

bool saveBlockDataToFile(std::vector<Block> blocks, int width, int height, const char* filename);

bool loadBlockDataFromFile(std::vector<Block> &blocks, int &width, int &height, const char* filename);