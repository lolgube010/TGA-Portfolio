
// yo.
// yoyo.
// listen here.
// this hlsli is exclusively for common buffers. nothing more.
// go to Common.hlsli for defines and functions.

Texture2D aTexture : register(t0);

SamplerState DefaultSampler : register(s0); // default tgp

SamplerState PointRepeat : register(s1); 
SamplerState LinearRepeat : register(s2); 
SamplerState PointClamp : register(s3);
SamplerState LinearClamp : register(s4);


cbuffer FrameBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float4x4 cameraMatrix;
    float4x4 projectionMatrix;
    float3 cameraPos;
    float time;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelToWorld;
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
    uint Instance : SV_InstanceID;
};

struct PixelInput
{
    float4 position : SV_POSITION0;
    float4 worldPosition : POSITION0;
    float4 worldNormal : NORMAL_WORLD;
    float4 vertexObjectNormal : NORMAL_VERTEX_OBJECT;
    float4 worldBitangent : BITANGENT_WORLD;
    float4 worldTangent : TANGENT_WORLD;
    float4 vertexColor : COLOR0;
    float2 uv : TEXCOORD0;
    uint Instance : SV_InstanceID;
};

struct PixelOutput
{
    float4 color : SV_TARGET0;
    // VI KAN SKRIVA SV TARGET0 också! det bestämmer vilket render target vi vill rita till
};