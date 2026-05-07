#include "assetManager.h"

void AssetManager::loadAll()
{
	dirt = LoadTexture(RESOURCES_PATH "dirt.png");
	castle = LoadTexture(RESOURCES_PATH "CastleTiles.png");
}