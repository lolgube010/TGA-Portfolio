#include "CommonBuffers.hlsli"
#include "PostprocessStructs.hlsli"
#include "PortfolioCommon.hlsli"
#include "Common.hlsli"

// SRV
Texture2D<float4> smokeAlbedoTex : register(t1);
Texture2D<half> smokeMaskTex : register(t2);
Texture2D<float> smokeDepthTex : register(t3);
Texture2D<float4> mainTex : register(t4); // copy of bb this frame.
Texture2D<float4> debugTex1 : register(t5);

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	float4 mainCol = mainTex.Sample(DefaultSampler, input.UV);
	float4 smokeAlbedo = smokeAlbedoTex.Sample(DefaultSampler, input.UV);
	float smokeMask = saturate(smokeMaskTex.Sample(DefaultSampler, input.UV).r);

	// applies some sharpness
	float neighbour = sharpness * -1;
	float center = sharpness * 4 + 1;

	// to get the texel size of a texture; Vector4(1 / width, 1 / height, width, height)
	// There’s more to it than that: _TexelSize.y is negative when it belongs to a RenderTexture that has been flipped vertically by D3D anti-aliasing. Unfortunately, even after years of documentation complaints and forum posts, this is the only thing that is documented about _TexelSize.
	uint2 mainTexTexelSize = uint2(1.f / backBufferSize);
	//mainTexTexelSize.y = 1 - mainTexTexelSize.y; // maybe flip

	float4 n = smokeAlbedoTex.Sample(DefaultSampler, input.UV + mainTexTexelSize.xy * float2(0, -1)); 
	float4 e = smokeAlbedoTex.Sample(DefaultSampler, input.UV + mainTexTexelSize.xy * float2(1, 0));  
	float4 s = smokeAlbedoTex.Sample(DefaultSampler, input.UV + mainTexTexelSize.xy * float2(0, 1));  
	float4 w = smokeAlbedoTex.Sample(DefaultSampler, input.UV + mainTexTexelSize.xy * float2(-1, 0)); 

	//float4 n = smokeAlbedoTex.Sample(DefaultSampler, input.UV + mainTexTexelSize.xy * float2(0, 1));
	//float4 e = smokeAlbedoTex.Sample(DefaultSampler, input.UV + mainTexTexelSize.xy * float2(1, 0));
	//float4 s = smokeAlbedoTex.Sample(DefaultSampler, input.UV + mainTexTexelSize.xy * float2(0, -1));
	//float4 w = smokeAlbedoTex.Sample(DefaultSampler, input.UV + mainTexTexelSize.xy * float2(-1, 0));

	float4 sharpenedSmoke = n * neighbour + e * neighbour + smokeAlbedo * center + s * neighbour + w * neighbour;

	PostProcessPixelOutput res;

	switch (debugView)
	{
	case 0: // composite
		//res.color = (smokeAlbedo);
		res.color = lerp(mainCol, saturate(sharpenedSmoke), 1 - smokeMask);

		if (activateTonemap)
		{
			res.color.xyz = tonemap_s_gamut3_cine(res.color);
		}
		return res;

	case 1:
		float adjustedMask = pow(smokeMask, maskInfluence);
		res.color = lerp(mainCol, saturate(sharpenedSmoke), 1 - adjustedMask);
		if (activateTonemap)
		{
			res.color.xyz = tonemap_s_gamut3_cine(res.color);
		}
		return res;
	case 2:
		float adjustedMask2 = pow(smokeMask, maskInfluence);
		res.color = lerp(mainCol, saturate(smokeAlbedo), 1 - adjustedMask2);
		if (activateTonemap)
		{
			res.color.xyz = tonemap_s_gamut3_cine(res.color);
		}
		return res;
	}
	res.color = float4(1, 0, 1, 0);
	return res;
}