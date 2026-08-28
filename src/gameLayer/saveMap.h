#pragma once
#include <vector>
#include <blocks.h>
#include <fstream>

bool saveBlockDataToFile(const std::vector<Block>& blocks, const std::vector<Block>& walls, int width, int height, const char* filename);

bool loadBlockDataFromFile(std::vector<Block> &blocks, std::vector<Block>& walls, int &width, int &height, const char* filename);