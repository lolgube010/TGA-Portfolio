#include "PortfolioCommon.hlsli"

#pragma region worley
float hash(uint n)
{
    // integer hash copied from Hugo Elias
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 0x789221U) + 0x1376312589U;
    return float(n & uint(0x7fffffffU)) / float(0x7fffffff);
}

float worley(float3 coord, int axisCellCount)
{
    int3 cell = floor(coord / cellSize);

    float3 localSamplePos = float3(coord / cellSize - cell);

    float dist = 1.0f;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            for (int z = -1; z <= 1; ++z)
            {
                int3 cellCoordinate = cell + int3(x, y, z);
                int x = cellCoordinate.x;
                int y = cellCoordinate.y;
                int z = cellCoordinate.z;

                if (x == -1 || x == axisCellCount || y == -1 || y == axisCellCount || z == -1 || z == axisCellCount)
                {
                    int3 wrappedCellCoordinate = fmod(cellCoordinate + axisCellCount, (int3) axisCellCount);
                    int wrappedCellIndex = wrappedCellCoordinate.x + axisCellCount * (wrappedCellCoordinate.y + wrappedCellCoordinate.z * axisCellCount);
                    float3 featurePointOffset = cellCoordinate + float3(hash(noiseSeed + wrappedCellIndex), hash(noiseSeed + wrappedCellIndex * 2), hash(noiseSeed + wrappedCellIndex * 3));
                    dist = min(dist, distance(cell + localSamplePos, featurePointOffset));
                }
                else
                {
                    int cellIndex = cellCoordinate.x + axisCellCount * (cellCoordinate.y + cellCoordinate.z * axisCellCount);
                    float3 featurePointOffset = cellCoordinate + float3(hash(noiseSeed + cellIndex), hash(noiseSeed + cellIndex * 2), hash(noiseSeed + cellIndex * 3));
                    dist = min(dist, distance(cell + localSamplePos, featurePointOffset));
                }
            }
        }
    }

    dist = sqrt(1.0f - dist * dist);
    dist *= dist * dist * dist * dist * dist;
    return dist;
}
#pragma endregion

#pragma region curl
uint pcg_hash(uint input)
{
    uint state = input * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float ConvertUintToFloat(uint value)
{
    return (float)value / (float)4294967295u; // Normalize to 0-1 range
}

#pragma endregion

RWTexture3D<half> RWWorleyNoiseTex : register(u0);
RWTexture3D<float3> RWCurlNoiseTex : register(u1);

[numthreads(8, 8, 8)]
void main(uint3 id : SV_DispatchThreadID)
{
#pragma region worleycode
	//Convert to UV
	float3 pos = float3(id.xyz);

	float n = 0.0f;

	float G = exp2(-1);
	float f = 1.0f;
	float a = 1.0f;

	for (int i = 0; i < octaves; ++i) 
	{
		float noise = worley(pos.xyz * f + i * warp, axisCellCount * f) * a;
		n += noise;
		f *= 2.0f;
		a *= G;
	}

	n += add;
	n = saturate(n) * amplitude;

	float worley = invertNoise ? amplitude - n : n;

    RWWorleyNoiseTex[id.xyz] = worley;
#pragma endregion
}