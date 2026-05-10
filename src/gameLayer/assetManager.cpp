#include "assetManager.h"

void AssetManager::loadAll()
{
	textures = LoadTexture(RESOURCES_PATH "textures.png");
	frame = LoadTexture(RESOURCES_PATH "frame.png");
}