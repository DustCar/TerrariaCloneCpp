#include "helpers.h"
#include <fstream>

Rectangle getTextureAtlas(int x, int y, int cellSizePixelsX, int cellSizePixelsY)
{
	return Rectangle{(float)x * cellSizePixelsX, (float)y * cellSizePixelsY, (float)cellSizePixelsX, (float)cellSizePixelsY};
}

float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

bool readEntireFile(const char* fileName, std::string& data)
{
	std::ifstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	f.seekg(0, std::ios::end);
	size_t size = f.tellg();
	if (size < 0) { return false; }
	
	f.seekg(0, std::ios::beg);

	data.resize(size);

	f.read(data.data(), size);

	return true;
}

bool readEntireFile(const char* fileName, std::vector<char>& data)
{
	std::ifstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	f.seekg(0, std::ios::end);
	size_t size = f.tellg();
	if (size < 0) { return false; }

	f.seekg(0, std::ios::beg);

	data.resize(size);

	f.read(data.data(), size);

	return true;
}

bool writeEntireFile(const char* fileName, const void* data, size_t size)
{
	std::ofstream f(fileName, std::ios::binary);

	if (!f.is_open()) { return false; }

	f.write(static_cast<const char*>(data), size);

	return true;
}
