//Texture2D aTexture : register(t0); // used by models outside of this class
//SamplerState aSampler : register(s0); // used by models outside of this class

SamplerState PointRepeatSampler : register(s1);
SamplerState LinearRepeatSampler : register(s2);
SamplerState PointClampSampler : register(s3);
SamplerState LinearClampSampler : register(s4);


//RWTexture3D<half> RWNoiseTex;
//Texture3D<half> NoiseTex;

//RWTexture2D<float4> SmokeTex/* : register(t1)*/;
//RWTexture2D<float> SmokeDepthTex/* : register(t1)*/;
//RWTexture2D<float4> SmokeMaskTex/* : register(t1)*/;

//Texture2D<half> Depth;

// fake enum: 
static const uint VOXEL_EMPTY = 0;
static const uint VOXEL_OCCUPIED = 1;
static const uint VOXEL_FILLED = 2; // anything above this == voxel is filled
static const uint VOXEL_SEED = 16; // todo- send through cb?

//steps = 256
//minHitdist = 0.001
//maxDist = 200

// TODO- set up some kind of standard for the way cbuffer elements are named so you don't end up double-naming all of them. eg: starting with _ or something
cbuffer PortfolioBuffer : register(b2)
{
    int3 voxelResolution; // used for calculating the voxelSpacePos of each voxel (to move the instance) (60, something)
    float voxelSize; // used for going from voxel to world space
    float3 boundingBoxExtents; // used for going from voxel to world space (15, 2.5, 15)
    uint voxelCount; // getting the max of our vector
    float3 smokePos;
    float smokeTimer;
    float3 smokeRadius;
    uint meshesToBeVoxelizedTriCount;

    bool drawSmokeVoxels;
    bool drawStaticVoxels;
    bool drawEdgeVoxels;
    bool drawSmokeSeed;

    uint2 backBufferSize;
    float absorptionCoefficient, scatteringCoefficient;

    float3 smokeColor /*, _ExtinctionColor*/;
    float _stepSize;

    float lightStepSize;
    float alphaThreshold;
    int _stepCount;
    float volumeDensity;

    float densityFalloff;
    float smokeSize;
    int octaves;
    float warp;

    float3 animDirection;
    int axisCellCount;

    float add;
    float amplitude;
    bool invertNoise;
    int cellSize;

    int noiseSeed;
    float sharpness; // used by composite
    int lightStepCount;
    int debugView;

    float3 sunPos;
    float _scatteringAnisotropy; // also called _G!

    int phaseFunc;
    float shadowDensity;
    int smokeRes;
    bool activateTonemap;

    float RayMarchGrowthSteepness;
    float3 extinctionColor;

    float voxelGrowthSteepness;
    float3 sunColor;

    float InterleavedGradientNoiseStrength;
    float maskInfluence;
    float biTagentNoiseScale;
    float biTagentNoiseStrength;

    // flaot3 bulletForward
    // float3 bulletOrigin
    // float r1
    // float r2
    // float bulletDepth
}

struct VoxelInstance
{
    uint myVoxelState; // Empty, filled by smoke, occupied by geometry
    float3 color; // TODO- REMOVE!
};

//struct SmokeData // i'll deal with this later on when and if i want multiple smoke instances
//{
//    uint fuel;
//    uint3 padd;
//};

// from voxel space to array space
uint VoxelToArray(uint3 pos)
{
    return pos.x + pos.y * voxelResolution.x + pos.z * voxelResolution.x * voxelResolution.y;
}

// from array space to voxel space
uint3 ArrayToVoxel(uint idx)
{
    uint x = idx % (voxelResolution.x);
    uint y = (idx / voxelResolution.x) % voxelResolution.y;
    uint z = idx / (voxelResolution.x * voxelResolution.y);

    return uint3(x, y, z);
}

bool IsVoxelFilled(uint aVoxelState)
{
    return aVoxelState >= VOXEL_FILLED && aVoxelState <= VOXEL_SEED;
}

float3 AcerolaVoxelToWorld(float3 aVoxelPos)
{
    // this is what's covered in the video
    return (aVoxelPos * voxelSize - boundingBoxExtents);

    // this is what's in voxelize.compute, and probably more correct
    float3 worldPos = aVoxelPos * voxelSize + (voxelSize * .5f);
    worldPos.xz - boundingBoxExtents.xz;
    return worldPos;
}

float3 momoVoxelToWorld(float3 aVoxelPos)
{
    float halfVoxelSize = voxelSize * .5f;
    float3 worldPos = aVoxelPos * voxelSize + halfVoxelSize - float3(boundingBoxExtents.x, 0, boundingBoxExtents.z);
    return worldPos;
}


uint3 AcerolaWorldToVoxel(float3 aWorldPos)
{
    float3 pos = aWorldPos % boundingBoxExtents;
    pos.xz += boundingBoxExtents.xz;
    pos /= boundingBoxExtents * 2;
    pos *= voxelResolution;
    return uint3(pos);
}

uint3 momoWorldToVoxel(float3 aWorldPos)
{
	// world to voxel-space
    float3 voxelPos = (((aWorldPos % boundingBoxExtents) + float3(boundingBoxExtents.x, 0, boundingBoxExtents.z)) / (boundingBoxExtents * 2)) * voxelResolution;
    return (uint3) voxelPos;
}

#ifndef PI
#define PI          3.14159265358979323846
#endif

float HenyeyGreenstein(float g, float cos_theta)
{
    float denom = 1.0f + g * g - 2.0f * g * cos_theta;
    return (1.0f / (4.0f * PI)) * ((1.0f - g * g) / (denom * sqrt(denom)));
}

float Mie(float g, float cos_theta)
{
    float denom = 1.0f + g * g - 2.0f * g * cos_theta;
    return (3.0f / (8.0f * PI)) * (((1 - g * g) * (1 + cos_theta * cos_theta)) / ((2.0f + g * g) * (denom * sqrt(denom))));
}

float Rayleigh(float cos_theta)
{
    return (3.0f / (16.0f * PI)) * (1 + cos_theta * cos_theta);
}