#include "Common.hlsli"
#include "CommonBuffers.hlsli"
#include "PortfolioCommon.hlsli"

StructuredBuffer<VoxelInstance> voxelStuff : register(t0);

PixelOutput main(PixelInput input)
{
	PixelOutput result;

	//if (voxelStuff[input.Instance].color.x == 500.f)
	//{
	//	result.color.xyz = float3(0, 1.f, 0.);
	//	result.color.a = 1.f;
	//	return result;
	//}
	//discard;
	if (drawEdgeVoxels)
	{
		//float3 ndotl = DotClamped(sunPos.xyz, input.worldNormal) * 0.5f + 0.5f;
		//ndotl *= ndotl;

		//result.color = float4(getRandom(input.Instance).xyz * ndotl, 1.0f);
		result.color.xyz = getRandom(input.Instance);
		result.color.a = 1.f;
		return result;
	}
	if (voxelStuff[input.Instance].myVoxelState == VOXEL_EMPTY) // voxel is empty
	{
		discard;
	}
	if (voxelStuff[input.Instance].myVoxelState == VOXEL_OCCUPIED && drawStaticVoxels) // voxel is filled by terrain
	{
		result.color = float4(0, 0, 0, 1);
		return result;
		//discard;
	}
	if (IsVoxelFilled(voxelStuff[input.Instance].myVoxelState) && drawSmokeVoxels) // voxel is filled by smoke
	{
		result.color = input.vertexColor;
		return result;
	}

	//result.color = input.color * input.position;
	discard;
	result.color = input.vertexColor;
	return result;
}