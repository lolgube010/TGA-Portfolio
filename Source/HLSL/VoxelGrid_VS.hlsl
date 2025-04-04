#include "CommonBuffers.hlsli"
#include "Common.hlsli"
// positions as vec4's: {x,y,z,1.f}
// normals as vec4's: {v,y,z,0.f}

#include "PortfolioCommon.hlsli"

StructuredBuffer<VoxelInstance> voxelStuff : register(t0);

PixelInput main(VertexInputType input)
{
	PixelInput output;

	float3 voxelGridPos = ArrayToVoxel(input.Instance); // pos in voxelGrid

	float3 bbExtentsButNoY = float3(boundingBoxExtents.x, 0, boundingBoxExtents.z);
	float4 newPos = float4((input.position.xyz + voxelGridPos) * voxelSize + (voxelSize * .5f) - bbExtentsButNoY, input.position.w);

	float4 vertexWorldPos = mul(modelToWorld, newPos); // this is how we do it in the normal mesh vs, no idea why below also works but watch out ig
	//float4 vertexWorldPos = mul(newPos, modelToWorld);
	float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
	output.position = vertexClipPos;

	output.vertexColor = float4(voxelStuff[input.Instance].color, 1.f);
	output.Instance = input.Instance; // todo remove? isn't it a SV for the PS too?

	//output.vertexObjectNormal = float4(input.normal, 0.0);
	//output.worldNormal = mul(modelToWorld, output.vertexObjectNormal);

	return output;
}