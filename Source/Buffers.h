#pragma once
#include <DirectXMath.h>

#include "CommonUtilities/Matrix4x4.h"
#include "ddsTexLoader/DDSTextureLoader11.h"

// TODO check pass order: https://learn.microsoft.com/en-us/windows/win32/dxtecharts/direct3d10-frequently-asked-questions
// the order these update in is incredibly important!!

struct alignas(16) FrameBufferData // updates per frame // (GI values, lights)
{
	CommonUtilities::Matrix4x4f worldToClipMatrix; // TODO- combine model and view matrix- keep projection seperate https://stackoverflow.com/questions/10617589/why-would-it-be-beneficial-to-have-a-separate-projection-matrix-yet-combine-mod
	CommonUtilities::Matrix4x4f cameraMatrix;
	CommonUtilities::Matrix4x4f projectionMatrix;
	CommonUtilities::Vector3f camPos;
	float time;

};

// ● One for per-object constants (World matrix,dynamic material properties, etc)
struct alignas(16) ObjectBufferData // updates per object
{
	CommonUtilities::Matrix4x4f modelToWorldMatrix; // TODO- combine model and view matrix- keep projection seperate
};

struct alignas(16) PortfolioBufferData // cbuffer!
{
	DirectX::XMINT3 boundingBoxGridRes;
	float voxelSize;

	DirectX::XMFLOAT3 boundingBoxExtents;
	unsigned int voxelCount;

	DirectX::XMFLOAT3 smokePos;
	float smokeTimer;

	DirectX::XMFLOAT3 smokeRadius;
	unsigned int meshesToBeVoxelizedTriCount;

	BOOL drawSmokeVoxels;
	BOOL drawStaticVoxels;
	BOOL drawEdgeVoxels;
	BOOL drawSmokeSeed;

	DirectX::XMUINT2 backBufferSize;
	float absorptionCoefficient, scatteringCoefficient;

	DirectX::XMFLOAT3 smokeColor;
	float stepSize;

	float lightStepSize;
	float alphaThreshold;
	int stepCount;
	float volumeDensity;

	float densityFallOff;
	float smokeSize;
	int octaves;
	float warp;

	DirectX::XMFLOAT3 animDirection;
	int axisCellCount;

	float add;
	float amplitude;
	BOOL invertNoise;
	int cellSize;

	int noiseSeed;
	float sharpness;
	int lightStepCount;
	int debugView;

	DirectX::XMFLOAT3 sunPos;
	float scatteringAnisotropy; // aka _G

	int phaseFunc;
	float shadowDensity;
	int smokeRes;
	BOOL activateTonemap;

	float rayMarchGrowthSteepness;
	DirectX::XMFLOAT3 extinctionColor;

	float voxelGrowthSteepness;
	DirectX::XMFLOAT3 sunColor;

	float InterleavedGradientNoiseStrength;
	float maskInfluence;
	float biTagentNoiseScale;
	float biTagentNoiseStrength;
};
struct alignas(16) SmokeBufferData
{
	unsigned int totalFuel; // fuel for the entirety of the smoke grenade
	unsigned int padding1;
	unsigned int padding2;
	unsigned int padding3;
};