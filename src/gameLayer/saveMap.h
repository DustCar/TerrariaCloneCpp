#pragma once
#include <vector>
#include <blocks.h>
#include <fstream>

// TODO: must include wall blocks to the functions
bool saveBlockDataToFile(std::vector<Block> blocks, int width, int height, const char* filename);

bool loadBlockDataFromFile(std::vector<Block> &blocks, int &width, int &height, const char* filename);