#include "Common.hlsli"
#include "CommonBuffers.hlsli"
#include "PBRFunctions.hlsli"

Texture2D aTestTexture : register(t2);
Texture2D aGrass_C : register(t3);
Texture2D aGrass_N : register(t4);
Texture2D aGrass_M : register(t9);
Texture2D aRock_C : register(t5);
Texture2D aRock_N : register(t6);
Texture2D aRock_M : register(t10);
Texture2D aSnow_C : register(t7);
Texture2D aSnow_N : register(t8);
Texture2D aSnow_M : register(t11);
TextureCube aCubeMap : register(t12);

float3 EvaluateAmbiance(TextureCube lysBurleyCube, float3 vN, float3 VNUnit, float3 toEye, float perceptualRoughness, float ao, float3 dfcol, float3 spccol)
{
    int numMips = GetNumMips(lysBurleyCube);
    const int nrBrdMips = numMips - nMipOffset;
    float VdotN = saturate(dot(toEye, vN)); //clamp(dot(toEye, vN), 0.0, 1.0f);
    const float3 vRorg = 2 * vN * VdotN - toEye;

    float3 vR = GetSpecularDominantDir(vN, vRorg, RoughnessFromPerceptualRoughness(perceptualRoughness));
    float RdotNsat = saturate(dot(vN, vR));

    float mipLevel = BurleyToMip(perceptualRoughness, numMips, RdotNsat);

    float3 specRad = lysBurleyCube.SampleLevel(DefaultSampler, vR, mipLevel).xyz;
    float3 diffRad = lysBurleyCube.SampleLevel(DefaultSampler, vN, (float) (nrBrdMips - 1)).xyz;

    float fT = 1.0 - RdotNsat;
    float fT5 = fT * fT;
    fT5 = fT5 * fT5 * fT;
    spccol = lerp(spccol, (float3) 1.0, fT5);

    float fFade = GetReductionInMicrofacets(perceptualRoughness);
    fFade *= EmpiricalSpecularAO(ao, perceptualRoughness);
    fFade *= ApproximateSpecularSelfOcclusion(vR, VNUnit);

    float3 ambientdiffuse = ao * dfcol * diffRad;
    float3 ambientspecular = fFade * spccol * specRad;

    return ambientdiffuse + ambientspecular;
}

PixelOutput main(PixelInput input)
{
    PixelOutput result;

    // momo ver
 //   float3 materialColor = { .1, .3, .1 };
 //   float3 directionalLightColor = { 1.f, 1.f, 1.f };
 //   float3 lightDir = normalize(float3(35, 90., 35));
 //   float3 normal = normalize(input.worldNormal);

 //   float3 directionalLight = materialColor * directionalLightColor * max(0.f, dot(lightDir, normal));

 //   // normal ambient:
 //   float3 ambientLightColor = { .8, .4, 0. };
 //   float3 ambientLight = materialColor * ambientLightColor;

 //   // two colored ambient
 //   float3 skyColor = { .4, .5, .3 };
 //   float3 groundColor = { .3, .3, .1 };
 //   float3 twoColoredAmbient = materialColor * ((0.5f + 0.5f * normal.y) * skyColor + (.5f - .5f * normal.y) * groundColor);

 //   //result.color = input.worldNormal;

	////result.color.rgb = directionalLight;
 //   //result.color.rgb = ambientLight;
 //   //result.color.rgb = directionalLight + ambientLight;
 //   //result.color.rgb = twoColoredAmbient;
 //   //result.color.rgb = directionalLight + ambientLight + twoColoredAmbient;
 //   result.color.rgb = directionalLight + twoColoredAmbient;

    float loopScale = 3.f;

    float3 worldNormal = normalize(input.worldNormal);
    float3 tangent = normalize(input.worldBitangent.xyz);
    float3 biTangent = normalize(input.worldNormal.xyz); // this should be replaced with the cross product calc instead.
    float3x3 TBN = float3x3(
		normalize(tangent),
		normalize(-biTangent),
		normalize(worldNormal)
		);
    TBN = transpose(TBN);

    // TODO- instead of calculating the bitangent just use the tangent and worldnormal??
	//    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	//vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	//// re-orthogonalize T with respect to N
	//    T = normalize(T - dot(T, N) * N);
	//// then retrieve perpendicular vector B with the cross product of T and N
	//vec3 B = cross(N, T);
	//mat3 TBN = mat3(T, B, N)
	float3 toEye = normalize(cameraPos - input.worldPosition.xyz);

	// light settings
    float3 lightDir = normalize(float3(1.0f, 1.0f, -1.0f));
    float3 directColor = float3(1.0f, 0.8f, 0.4f);
    float3 ambientColor = float3(0.05f, 0.1f, 0.3f);

    //float3 testNormal = expandNormal(aTestTexture.Sample(aSampler, 3.f * input.uv.xy));

    // GRASS
	float4 grassColor = aGrass_C.Sample(DefaultSampler, loopScale * input.uv.xy);
    float3 grassNormal = expandNormal(aGrass_N.Sample(DefaultSampler, loopScale * input.uv.xy));
    float3 grassMat = aGrass_M.Sample(DefaultSampler, loopScale * input.uv.xy);
    float grass_AO = grassMat.r; // TODO- REMOVE
    float grass_Rough = grassMat.g; // TODO- REMOVE
    float grass_Metal = grassMat.b; // TODO- REMOVE

    // ROCK
    float4 rockColor = aRock_C.Sample(DefaultSampler, loopScale * input.uv.xy);
    float3 rockNormal = expandNormal(aRock_N.Sample(DefaultSampler, loopScale * input.uv.xy));
    float3 rockMat = aRock_M.Sample(DefaultSampler, loopScale * input.uv.xy);
    float rock_AO = rockMat.r; // TODO- REMOVE
    float rock_Rough = rockMat.g; // TODO- REMOVE
    float rock_Metal = rockMat.b; // TODO- REMOVE

    // SNOW
    float4 snowColor = aSnow_C.Sample(DefaultSampler, loopScale * input.uv.xy);
    float3 snowNormal = expandNormal(aSnow_N.Sample(DefaultSampler, loopScale * input.uv.xy));
    float3 snowMat = aSnow_M.Sample(DefaultSampler, loopScale * input.uv.xy);
    float snow_AO = snowMat.r; // TODO- REMOVE
    float snow_Rough = snowMat.g; // TODO- REMOVE
    float snow_Metal = snowMat.b; // TODO- REMOVE

    // BLENDS
    float slopeBlend = smoothstep(0.7f, 1.0f, input.worldNormal.y);
    float heightBlend = smoothstep(-.05f, .25f, input.worldPosition.y);
    //float heightBlend = lerp(-.05f, .25f, input.worldPosition.y);

    // COMBINING
    float3 normal = lerp(rockNormal, lerp(grassNormal, snowNormal, heightBlend), slopeBlend);
    normal = normalize(mul(TBN, normal));
	float3 color = lerp(rockColor, lerp(grassColor, snowColor, heightBlend), slopeBlend).rgb;
	float3 ambientOcclusion = lerp(rock_AO, lerp(grass_AO, snow_AO, heightBlend), slopeBlend);
    float3 roughness = lerp(rock_Rough, lerp(grass_Rough, snow_Rough, heightBlend), slopeBlend);
    float3 metalness = lerp(rock_Metal, lerp(grass_Metal, snow_Metal, heightBlend), slopeBlend);


    // RESULT
    float3 diffuse = lerp((float3) 0.00f, color.rgb, 1 - metalness);
    //float3 diffuse = float3(1.f,1.f,1.f);
    //float3 diffuse = float3(0.f,0.f,0.f);
    float3 specular = lerp((float3) 0.04f, color.rgb, metalness);
    //float3 specular = float3(0.f,0.f,0.f);;

	float3 ambiance = EvaluateAmbiance(
		aCubeMap, normal, input.worldNormal.xyz,
		toEye, roughness,
		ambientOcclusion, diffuse, specular
	);

    float3 directionalLight = EvaluateDirectionalLight(
		diffuse, specular, normal, roughness,
		directColor, lightDir, toEye.xyz
	);

    float3 resultColor = ambiance + directionalLight;

    // gammal ljussättning
    //result.color.rgb = color * ((0.5f + 0.5f * normal.y) * ambientColor + directColor * max(0.f, dot(normal, lightDir)));

    result.color.rgb = tonemap_s_gamut3_cine(resultColor);
    result.color.a = 1.f;

    return result;
}