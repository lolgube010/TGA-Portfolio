#include "CommonBuffers.hlsli"

PixelOutput main(PixelInput input)
{
	PixelOutput result;

	float4 color1 = float4(0.05, 0.05, 0.05, 1.f);
	float4 color2 = float4(0.1, 0.1, 0.1, 1.f);
	float tileSize = .25f;
	//result.color = aTexture.Sample(DefaultSampler, input.uv.xy).rgba;
	float2 tile = floor(input.uv / tileSize);

	float checker = fmod(tile.x + tile.y, 2.f);
	float4 baseCol = lerp(color1, color2, checker);

	float3 normal = input.worldNormal.xyz;
	
	float3 lightDir = normalize(float3(-1.f, -1.f, -1.f));
	float diffuse = max(0.0f, dot(normal, -lightDir));
	//result.color = lerp(color1, color2, checker);
	//result.color = float4(input.uv, 1.f, 1.f);
	//result.color = baseCol * (diffuse + 0.2);

	//result.color.xyz = input.vertexObjectNormal;
	//result.color.a = 1.f;

	float pseudoLight = saturate(input.worldPosition.y * 0.1); // Scale y for effect
    float shading = pseudoLight + 0.2; // Add ambient
    
    // Apply shading
    float4 finalColor = baseCol * shading;
    finalColor.a = baseCol.a; // Preserve alpha

	result.color = finalColor;
	return result;
}