#include "PostprocessStructs.hlsli"

SamplerState PointClamp : register(s3);
Texture2D cameraDepth : register(t1);

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;
	result.color.r = cameraDepth.Sample(PointClamp, input.UV).r;
	result.color.gba = float3(0, 0, 1.f);
	return result;
}