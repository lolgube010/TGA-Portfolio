#include "CommonBuffers.hlsli"

// positions as vec4's: {x,y,z,1.f}
// normals as vec4's: {v,y,z,0.f}

PixelInput main(VertexInputType input)
{
    PixelInput output;
    float4 vertexObjectPos = input.position; // this will be used later
    float4 vertexObjectNormal = float4(input.normal, 0.0); // TODO- SEND TO PS

    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos); // this will be used later
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);

    float4 worldNormal = mul(modelToWorld, vertexObjectNormal); // TODO- SEND TO PS
    float4 worldTangent = mul(modelToWorld, input.tangent);
    float4 worldBiTangent = mul(modelToWorld, input.biTangent);

    output.position = vertexClipPos;
    output.uv = input.uv;
    output.vertexObjectNormal = vertexObjectNormal;
    output.worldPosition = vertexWorldPos;
    output.worldNormal = worldNormal;
    output.worldTangent = worldTangent;
    output.worldBitangent = worldBiTangent;
    //output.clip = vertexWorldPos.y - clipHeight;

    return output;
}