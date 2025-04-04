#include "PortfolioCommon.hlsli"

RWStructuredBuffer<VoxelInstance> voxelBuff : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	uint index = VoxelToArray(uint3(id.x, id.y, id.z)); // position in voxel space to array index
	if (voxelBuff[index].myVoxelState == VOXEL_OCCUPIED)
	{
		return;
	}
	voxelBuff[index].myVoxelState = VOXEL_EMPTY;
	voxelBuff[index].color = float3(0,0,0);
}
