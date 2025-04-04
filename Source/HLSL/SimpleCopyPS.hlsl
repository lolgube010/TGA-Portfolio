#include "CommonBuffers.hlsli"
#include "PostprocessStructs.hlsli"
#include "PortfolioCommon.hlsli"

// SRV
Texture2D source : register(t5); // copy of bb this frame.

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput op;
	op.color = source.Sample(DefaultSampler, input.UV);
	return op;
}