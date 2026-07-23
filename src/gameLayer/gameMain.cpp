#include <raylib.h>
#include <iostream>
#include "gameMain.h"
#include <asserts.h>
#include <assetManager.h>
#include <gameMap.h>
#include <helpers.h>
#include <imgui.h>
#include <raymath.h>
#include <randomHelpers.h>
#include <worldGenerator.h>

void ProcessMouseInput(int blockX, int blockY);
void DrawImGui(float& cameraZoom, float& cameraSpeed);
void DrawNoiseSettings(WorldParameters::NoiseParameters& noiseParams, const std::string& noiseLabel, bool bHasNoisePower = true);
void CreateSelectableComboList(const char* label, const char** options, int opCount, int* currentIndex);


// sure this is a global struct, but it's only global in this cpp file
struct GameData
{
	GameMap gameMap;
	Camera2D camera = Camera2D{};

	int selectedBlock = Block::dirt;

	std::ranlux24_base rng;

}gameData;

AssetManager assetManager;

bool InitGame()
{

	assetManager.loadAll();
	GenerateWorld(gameData.gameMap);

	// store the random number generator
	gameData.rng = std::ranlux24_base(std::random_device{}());

	gameData.camera.target = { 100, 100 }; // world-space center of view; will be used as camera position
	gameData.camera.rotation = 0.f;
	gameData.camera.zoom = 10.f;

	return true;
}

bool UpdateGame()
{
	ImGuiIO& io = ImGui::GetIO();

	float deltaTime = GetFrameTime();
	// cap deltaTime at 5 frames per second
	if (deltaTime > 1.f / 5.f) { deltaTime = 1.f / 5.f; }

	gameData.camera.offset = { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };

	ClearBackground({ 75, 75, 150, 255 });
	
/* camera movement begin */
	static float CAMERA_SPEED = 100.f;
	if (IsKeyDown(KEY_A)) { gameData.camera.target.x -= CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_D)) { gameData.camera.target.x += CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_W)) { gameData.camera.target.y -= CAMERA_SPEED * deltaTime; }
	if (IsKeyDown(KEY_S)) { gameData.camera.target.y += CAMERA_SPEED * deltaTime; }
/* camera movement end */


/* mouse controls begin */
	Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), gameData.camera);
	int blockX = (int)floor(worldPos.x);
	int blockY = (int)floor(worldPos.y);

	if (!io.WantCaptureMouse)
	{
		ProcessMouseInput(blockX, blockY);
	}
/* mouse controls end */


/* draw world begin */
	BeginMode2D(gameData.camera);

	// only draw blocks within the screen dimensions
	Vector2 topLeftView = GetScreenToWorld2D({ 0, 0 }, gameData.camera);
	Vector2 bottomRightView = GetScreenToWorld2D({ (float)GetScreenWidth(), (float)GetScreenHeight() }, gameData.camera);

	// padding
	int startXView = (int)floorf(topLeftView.x - 1.f);
	int endXView = (int)ceilf(bottomRightView.x + 1.f);
	int startYView = (int)floorf(topLeftView.y - 1.f);
	int endYView = (int)ceilf(bottomRightView.y + 1.f);


	startXView = Clamp(startXView, 0, gameData.gameMap.w - 1);
	endXView = Clamp(endXView, 0, gameData.gameMap.w - 1);

	startYView = Clamp(startYView, 0, gameData.gameMap.h - 1);
	endYView = Clamp(endYView, 0, gameData.gameMap.h - 1);

	// y outer, x inner for right-down iteration like old tvs
	// draw blocks into world
	for (int y = startYView; y <= endYView; y++)
		for (int x = startXView; x <= endXView; x++)
		{
			
			// walls; drawn first so blocks go in front
			auto& w = gameData.gameMap.getWallUnsafe(x, y);

			if (w.type != Block::air)
			{
				DrawTexturePro(
					assetManager.textures, // texture
					getTextureAtlas(w.type, w.variant, 32, 32), // source
					{ (float)x, (float)y, 1, 1 }, // dest
					{ 0, 0 }, // origin of texture (rotation/scale point) (top-left corner)
					0.f, // rotation
					WHITE // tint
				);
			}

			// blocks
			auto& b = gameData.gameMap.getBlockUnsafe(x, y);

			if (b.type != Block::air)
			{
				// draw wood log situationally
				if (b.type == Block::woodLog)
				{
					auto bUp = gameData.gameMap.getBlockSafe(x, y - 1);
					auto bDown = gameData.gameMap.getBlockSafe(x, y + 1);
					auto bRight = gameData.gameMap.getBlockSafe(x + 1, y);
					auto bLeft = gameData.gameMap.getBlockSafe(x - 1, y);

					int treeType = 0;
					if (bDown->type == Block::woodLog || bDown->type == Block::leaves)
					{
						treeType = 0;
						if (bUp->type == Block::leaves)
						{
							treeType = 5;
						}
						else if (bRight->type == Block::leaves && bLeft->type == Block::leaves)
						{
							treeType = 1;
						}
						else if (bRight->type == Block::leaves)
						{
							treeType = 2;
						}
						else if (bLeft->type == Block::leaves)
						{
							treeType = 3;
						}
						else if (bUp->type == Block::air)
						{
							treeType = 6;
						}
					}
					else
					{
						if (bUp->type == Block::air)
						{
							treeType = 7;
						}
						else
						{
							treeType = 4;
						}
					}

					DrawTexturePro(
						assetManager.tree, // texture
						getTextureAtlas(treeType, b.variant, 32, 32), // source
						{ (float)x, (float)y, 1, 1 }, // dest
						{ 0, 0 }, // origin
						0.f, // rotation
						WHITE // tint
					);
				}
				// draw everything else
				else
				{
					// possible PRNG alternative to current random() algo used for b.variant
					/*int szudzikValue = x >= y ? (x * x) + x + y : (y * y) + x;
					int rand = randomWithSeed(szudzikValue, 0, 3);*/

					DrawTexturePro(
						assetManager.textures, // texture
						getTextureAtlas(b.type, b.variant, 32, 32), // source
						{ (float)x, (float)y, 1, 1 }, // dest
						{ 0, 0 }, // origin of texture (rotation/scale point) (top-left corner)
						0.f, // rotation
						WHITE // tint
					);
				}
			}
		
		}

	// draw frame for block placement
	if (!io.WantCaptureMouse)
	{
		DrawTexturePro(
			assetManager.frame, // texture
			{ 0, 0, (float)assetManager.frame.width, (float)assetManager.frame.height }, // source
			{ (float)blockX, (float)blockY, 1, 1 }, // dest
			{ 0,0 }, // origin
			0.f, // rotation
			WHITE // tint
		);
	}

	EndMode2D();
/* draw world end */


/* simple imgui begin */
#if PRODUCTION_BUILD == 0
	DrawImGui(gameData.camera.zoom, CAMERA_SPEED);
#endif
/* simple imgui end */

	DrawFPS(10, 10);

	return true;
}

void CloseGame()
{

}

void ProcessMouseInput(int blockX, int blockY)
{
	// bool to avoid destroying walls behind blocks immediately
	static bool bDestroyingBlocks = false;

	// break block
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
	{
		auto b = gameData.gameMap.getBlockSafe(blockX, blockY);

		if (b && b->type != Block::air)
		{
			*b = {};
			bDestroyingBlocks = true;
		}
		else
		{
			auto w = gameData.gameMap.getWallSafe(blockX, blockY);
			if (w && !bDestroyingBlocks)
			{
				*w = {};
			}
		}

	}

	// reset destroying blocks bool
	if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON) && bDestroyingBlocks)
	{
		bDestroyingBlocks = false;
	}

	// place block
	if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	{
		// placement of standard blocks
		if (gameData.selectedBlock <= Block::emeraldBlock)
		{
			auto b = gameData.gameMap.getBlockSafe(blockX, blockY);
			// check adjacent blocks
			auto bUp = gameData.gameMap.getBlockSafe(blockX, blockY - 1);
			auto bDown = gameData.gameMap.getBlockSafe(blockX, blockY + 1);
			auto bRight = gameData.gameMap.getBlockSafe(blockX + 1, blockY);
			auto bLeft = gameData.gameMap.getBlockSafe(blockX - 1, blockY);

			// only place if the block can attach to another block and if space is empty
			if ((bUp && bUp->type != Block::air) || (bDown && bDown->type != Block::air) || (bRight && bRight->type != Block::air) || (bLeft && bLeft->type != Block::air))
			{
				if (b && b->type == Block::air)
				{
					b->type = gameData.selectedBlock;
					b->variant = GetRandomInt(gameData.rng, 0, 3);
				}
			}
		}
		// placement of wall blocks
		else
		{
			// check to see if a block has not been placed
			auto block = gameData.gameMap.getBlockSafe(blockX, blockY);
			if (block->type == Block::air)
			{
				auto w = gameData.gameMap.getWallSafe(blockX, blockY);
				if (w && w->type == Block::air)
				{
					w->type = gameData.selectedBlock;
					w->variant = GetRandomInt(gameData.rng, 0, 3);
				}
			}
		}
	}

#if PRODUCTION_BUILD == 0

	gameData.camera.zoom += (int)(GetMouseWheelMove() * 1);
	gameData.camera.zoom = gameData.camera.zoom < 3 ? 3 : gameData.camera.zoom;
	gameData.camera.zoom = gameData.camera.zoom > 50 ? 50 : gameData.camera.zoom;

#endif
}

void DrawImGui(float& cameraZoom, float& cameraSpeed)
{
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Game Editor");

	ImGui::BeginTabBar("Tabs");
	if (ImGui::BeginTabItem("Block Selector"))
	{
		for (int i = 1; i < Block::BLOCKS_COUNT; i++)
		{
			auto atlas = getTextureAtlas(i, 0, 32, 32);
			// transform texture parameters into UV coords
			atlas.x /= assetManager.textures.width;
			atlas.width /= assetManager.textures.width;
			atlas.y /= assetManager.textures.height;
			atlas.height /= assetManager.textures.height;

			ImGui::PushID(i);

			ImTextureID tex = (ImTextureID)(intptr_t)assetManager.textures.id;
			if (ImGui::ImageButton("", tex, { 28, 28 },
				{ atlas.x, atlas.y }, { atlas.x + atlas.width, atlas.y + atlas.height }))
			{
				gameData.selectedBlock = i;
			}

			ImGui::PopID();

			if (i % 10 != 0)
			{
				ImGui::SameLine();
			}
		}
		ImGui::EndTabItem();
	}

	ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;
	if (ImGui::BeginTabItem("Camera Settings"))
	{
		ImGui::SliderFloat("Camera Zoom:", &cameraZoom, 3.f, 100.f, "%.3f", flags);
		ImGui::SliderFloat("Camera Speed:", &cameraSpeed, 10.f, 150.f, "%.3f", flags);

		ImGui::EndTabItem();
	}

	/* Tab for world generation settings */
	static WorldParameters worldParams;
	static int worldSeed = 1234;
	if (ImGui::BeginTabItem("World Parameters"))
	{
		ImGui::Text("World Settings");
		ImGui::Separator();

		// width and height
		ImGui::AlignTextToFramePadding(); ImGui::Text("width, height:"); ImGui::SameLine();

		ImGui::PushItemWidth(150.f);
		ImGui::InputInt("##width", &worldParams.width, 0, 0); ImGui::SameLine();
		ImGui::InputInt("##height", &worldParams.height, 0, 0);
		ImGui::PopItemWidth();

		// seed
		ImGui::AlignTextToFramePadding(); ImGui::Text("seed:"); ImGui::SameLine();
		
		ImGui::PushItemWidth(200.f);
		ImGui::InputInt("##worldSeed", &worldSeed, 0, 0);
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("R"))
		{
			worldSeed = GetRandomInt(std::ranlux24_base(std::random_device{}()), 0, (int)std::ranlux24_base::max());
		}

		// generator settings
		ImGui::NewLine();
		// dirt noise generator
		ImGui::SeparatorText("Dirt Noise Generator Settings");
		DrawNoiseSettings(worldParams.dirtParams, "dirt");

		// cave noise generator
		ImGui::SeparatorText("Cave Noise Generator Settings");
		DrawNoiseSettings(worldParams.caveParams, "cave", false);
		// cave noise 2 generator
		ImGui::SeparatorText("Cave Noise 2 Generator Settings");
		DrawNoiseSettings(worldParams.cave2Params, "cave2", false);

		ImGui::Checkbox("BlendThirdNoise", &worldParams.bBlendThirdCaveNoise);

		ImGui::BeginDisabled(!worldParams.bBlendThirdCaveNoise);

		// cave noise 3 generator
		ImGui::SeparatorText("Cave Noise 3 Generator Settings");
		DrawNoiseSettings(worldParams.cave3Params, "cave3", false);
		// cave noise blend generator
		ImGui::SeparatorText("Cave Noise Blend Generator Settings");
		DrawNoiseSettings(worldParams.caveBlendParams, "caveBlend", false);

		ImGui::EndDisabled();

		// generate world
		if (ImGui::Button("Re-generate"))
		{
			GenerateWorld(gameData.gameMap, worldParams, worldSeed);
		}

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();

	ImGui::End();
}

void DrawNoiseSettings(WorldParameters::NoiseParameters& noiseParams, const std::string& noiseLabel, bool bHasNoisePower)
{
	const char* noiseTypes[] = {"OpenSimplex2", "OpenSimplex2S", "Cellular", "Perlin", "ValueCubic", "Value"};
	const char* fractalTypes[] = { "None", "FBm", "Rigid", "PingPong" };
	const char* cellDistance[] = { "Euclidean", "EuclideanSq", "Manhattan", "Hybrid"};
	const char* cellReturn[] = { "CellValue", "Distance", "Distance2", "Distance2Add", "Distance2Sub", "Distance2Mul", "Distance2Div" };

	std::string typeLabel = "##" + noiseLabel + "Type";
	std::string frequencyLabel = "##" + noiseLabel + "Frequency";
	std::string noisePowerLabel = "##" + noiseLabel + "NoisePower";
	// fractal labels
	std::string fractalLabel = "##" + noiseLabel + "Fractal";
	std::string octavesLabel = "##" + noiseLabel + "Octaves";
	std::string lacunarityLabel = "##" + noiseLabel + "Lacunarity";
	std::string gainLabel = "##" + noiseLabel + "Gain";
	// cellular labels
	std::string cellDistLabel = "##" + noiseLabel + "Distance";
	std::string cellReturnLabel = "##" + noiseLabel + "Return";
	std::string cellJitterLabel = "##" + noiseLabel + "Jitter";

	ImGui::PushItemWidth(200.f);
	// noise type
	ImGui::AlignTextToFramePadding(); ImGui::Text("Noise Type:"); ImGui::SameLine();
	CreateSelectableComboList(typeLabel.c_str(), noiseTypes, IM_ARRAYSIZE(noiseTypes), &noiseParams.noiseType);

	// frequency
	ImGui::AlignTextToFramePadding(); ImGui::Text("Frequency:"); ImGui::SameLine();
	ImGui::InputFloat(frequencyLabel.c_str(), &noiseParams.frequency, 0.f, 0.f, "%.3f");

	// noise power
	if (bHasNoisePower)
	{
		ImGui::AlignTextToFramePadding(); ImGui::Text("Noise Power:"); ImGui::SameLine();
		ImGui::InputFloat(noisePowerLabel.c_str(), &noiseParams.noisePower, 0.f, 0.f, "%.3f");
	}

	ImGui::NewLine();
	ImGui::TextDisabled("Fractal Settings");

	// fractal type
	ImGui::AlignTextToFramePadding(); ImGui::Text("Fractal Type:"); ImGui::SameLine();
	CreateSelectableComboList(fractalLabel.c_str(), fractalTypes, IM_ARRAYSIZE(fractalTypes), &noiseParams.fractalType);

	// fractal octaves
	ImGui::AlignTextToFramePadding(); ImGui::Text("Octaves:"); ImGui::SameLine();
	ImGui::InputInt(octavesLabel.c_str(), &noiseParams.fractalOctaves, 0, 0);

	// fractal lacunarity
	ImGui::AlignTextToFramePadding(); ImGui::Text("Lacunarity:"); ImGui::SameLine();
	ImGui::InputFloat(lacunarityLabel.c_str(), &noiseParams.fractalLacunarity, 0.f, 0.f, "%.1f");

	// fractal gain
	ImGui::AlignTextToFramePadding(); ImGui::Text("Gain:"); ImGui::SameLine();
	ImGui::InputFloat(gainLabel.c_str(), &noiseParams.fractalGain, 0.f, 0.f, "%.1f");

	ImGui::NewLine();
	ImGui::TextDisabled("Cellular Settings");
	
	// only enable if noise type is "Cellular"
	ImGui::BeginDisabled(noiseParams.noiseType != 2);

	// cell distance function
	ImGui::AlignTextToFramePadding(); ImGui::Text("Distance Function:"); ImGui::SameLine();
	CreateSelectableComboList(cellDistLabel.c_str(), cellDistance, IM_ARRAYSIZE(cellDistance), &noiseParams.cellDistFunc);

	// cell return type
	ImGui::AlignTextToFramePadding(); ImGui::Text("Return Type:"); ImGui::SameLine();
	CreateSelectableComboList(cellReturnLabel.c_str(), cellReturn, IM_ARRAYSIZE(cellReturn), &noiseParams.cellReturnType);

	// cell jitter
	ImGui::AlignTextToFramePadding(); ImGui::Text("Jitter:"); ImGui::SameLine();
	ImGui::InputFloat(cellJitterLabel.c_str(), &noiseParams.cellJitter, 0.f, 0.f, "%.1f");

	// Cellular end
	ImGui::EndDisabled();

	ImGui::PopItemWidth();

	ImGui::NewLine();
}

void CreateSelectableComboList(const char* label, const char** options, int opCount, int* currentIndex)
{
	const char* preview = options[*currentIndex];
	if (ImGui::BeginCombo(label, preview))
	{
		for (int i = 0; i < opCount; i++)
		{
			const bool bIsSelected = (i == *currentIndex);

			if (ImGui::Selectable(options[i], bIsSelected))
				*currentIndex = i;

			if (bIsSelected)
				ImGui::SetItemDefaultFocus;
		}

		ImGui::EndCombo();
	}
}
