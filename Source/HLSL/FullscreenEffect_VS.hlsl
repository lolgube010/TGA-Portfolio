#include "PostprocessStructs.hlsli"

PostProcessVertexToPixel main(PostProcessVertexInput input)
{
    float4 pos[3] =
    {
        float4(-1.0f, -1.0f, 0.0f, 1.0f),
		float4(-1.0f, 3.0f, 0.0f, 1.0f),
		float4(3.0f, -1.0f, 0.0f, 1.0f)
    };

    float2 uv[3] =
    {
        float2(0.0f, 1.0f),
		float2(0.0f, -1.0f),
		float2(2.0f, 1.0f)
    };

    
    PostProcessVertexToPixel returnValue;
    returnValue.position = pos[input.index];
    returnValue.UV = uv[input.index];
    return returnValue;
}