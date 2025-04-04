#pragma once
#include "RayMarcher.h"

namespace PortfolioSettings
{
	inline bool drawVoxels = false;
	inline bool drawSmokeVoxels = false;
	inline bool drawSmokeSeed = false;
	inline bool drawStaticVoxels = false;
	inline bool drawEdgeVoxels = false;
	inline bool drawRayCube = false;
	inline bool vsync = true;
	inline bool wireframe = false;
	inline bool activateTonemap = true;
	inline constexpr float voxelSize = .5f;
	inline int debugView = 1;

	// noise settings
	inline int seed = 0;
	inline int octaves = 6;
	inline int cellSize = 32;
	inline int axisCellCount = 4;
	inline float amplitude = .62f;
	inline float warp = 0.76f;
	inline float add = 0.f;
	inline float ImGuiNoiseTexSize = 1.f;

	inline RayMarcherStructs::Res resScale = RayMarcherStructs::Res::Full;
	//inline RayMarcherStructs::Res resScale = RayMarcherStructs::Res::Quarter;

	inline bool invertNoise = true;
	inline bool updateNoise = false;

	inline bool debugNoise = false;
	inline bool debugTiledNoise = false;
	inline int debugWorleyNoiseSlice = 0; // used in imgui at least
	inline int debugCurlNoiseSlice = 0; // used in imgui at least
	inline RayMarcherStructs::DebugNoiseAxis debugNoiseAxis;

	inline int stepCount = 150;
	inline float stepSize = 0.05f;
	inline float lightStepSize = 0.25f;
	inline int lightStepCount = 16;
	inline float smokeSize = 4.f; // ??, moved to smoke struct?

	inline float volumeDensity = .2f;
	inline float absorptionCoefficient = 2.0f;
	inline float scatteringCoefficient = 2.6f;
	inline DirectX::XMFLOAT3 extinctionColor(1, 1, 1);
	inline float shadowDensity = .625f;
	inline RayMarcherStructs::PhaseFunction phaseFunc;
	inline float scatteringAnisotropy = 0.0f;
	inline float densityFalloff = 0.75f;
	inline float alphaThreshold = 0.1f;
	inline DirectX::XMFLOAT3 animDirection(0, -0.1f, 0);
	inline bool bicubicUpscale = true;
	inline float sharpness = -1.f;
	inline float rayMarchGrowthSteepness = .5f; // only for the smoke renderer.
	inline float voxelGrowthSteepness = .25f; // voxel growth
	inline float InterleavedGradientNoiseStrength = -1.455f;
	inline float maskInfluence = 10.f;
	inline float biTagentNoiseScale = 5.f;
	inline float biTagentNoiseStrength = 5.f;

	inline struct Smoke
	{
		DirectX::XMFLOAT3 pos = { 0,0,0 };
		DirectX::XMFLOAT3 radius = { 5,4,5 }; // width height / depth of elipsoid
		//DirectX::XMFLOAT3 color = { .5f,.465f,.043f };
		DirectX::XMFLOAT3 color = { 0.f,0.f,0.f };
		float timer = 0;
	} smoke;

	inline struct Sun
	{
		DirectX::XMFLOAT3 pos = { 0,20,0 };
		DirectX::XMFLOAT3 dir = { 50.f,35.79f,0 };
		DirectX::XMFLOAT3 color = { 3.5816120f,2.7608260f,1.4363760f };

	} sun;
}
