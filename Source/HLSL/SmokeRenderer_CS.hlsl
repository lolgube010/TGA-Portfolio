#include "Common.hlsli"
#include "CommonBuffers.hlsli"
#include "PortfolioCommon.hlsli"

// depthTex
// smokeTex
// smokeMaskTex

// UAV
RWTexture2D<float4> smokeAlbedoTex : register(u1);
RWTexture2D<half> smokeMaskTex : register(u2);

// SRV
Texture2D<float> smokeDepthTex : register(t1);
Texture3D<half> noiseTex : register (t2);
Texture3D<float3> curlTex : register (t3);
//StructuredBuffer<VoxelInstance> voxelBuff : register(t5);

// debug UAV
RWStructuredBuffer<VoxelInstance> voxelBuff : register(u4);
RWTexture2D<float4> debug1Tex : register(u5);
RWTexture2D<float4> debug2Tex : register(u6);
RWTexture2D<float4> debug3Tex : register(u7);

float3 GetClipToWorldPos(float2 ndc, float deviceDepth)
{
	float4 clipSpacePos = float4(ndc * 2 - 1, deviceDepth, 1.0);
	//clipSpacePos.y *= -1.f; // god bless bjorn- wise magic man. may his pillows always be cold

	float4x4 clipToWorld = Inverse(worldToClipMatrix);

	// homogeneous Pos WorldSpace
	float4 hPosWS = mul(clipToWorld, clipSpacePos); // clip to world = _CameraInvViewProjection
	return hPosWS.xyz / hPosWS.w; // perspective divide
}

int momoGetVoxel(float3 aWorldPos)
{
	int v = 0;
	aWorldPos.y -= boundingBoxExtents.y;

	if (abs(dot(aWorldPos, float3(1, 0, 0))) <= boundingBoxExtents.x &&
		abs(dot(aWorldPos, float3(0, 1, 0))) <= boundingBoxExtents.y &&
		abs(dot(aWorldPos, float3(0, 0, 1))) <= boundingBoxExtents.z)
	{
		aWorldPos.y += boundingBoxExtents.y;

		float3 resultPos = aWorldPos;
		resultPos.xz += boundingBoxExtents.xz;
		resultPos /= boundingBoxExtents * 2;
		resultPos *= voxelResolution;

		//uint3 resultPos = momoWorldToVoxel(aWorldPos); // to be honest i have no fucking clue as to why worldToVoxel doesn't work here. but whatevs ig.

		v = voxelBuff[VoxelToArray(resultPos)].myVoxelState;
	}
	return v;
}



float getTrilinearVoxel(float3 pos)
{
	float v = 0;
	pos.y -= boundingBoxExtents.y;

	if (abs(dot(pos, float3(1, 0, 0))) <= boundingBoxExtents.x &&
		abs(dot(pos, float3(0, 1, 0))) <= boundingBoxExtents.y &&
		abs(dot(pos, float3(0, 0, 1))) <= boundingBoxExtents.z)
	{
		pos.y += boundingBoxExtents.y;
		float3 seedPos = pos;
		seedPos.xz += boundingBoxExtents.xz;
		seedPos /= boundingBoxExtents * 2;
		seedPos *= voxelResolution;
		seedPos -= 0.5f;

		uint3 vi = floor(seedPos);

		float value = 0.0f;

		for (int i = 0; i < 2; ++i)
		{
			float weight1 = 1 - min(abs(seedPos.x - (vi.x + i)), voxelResolution.x);
			for (int j = 0; j < 2; ++j)
			{
				float weight2 = 1 - min(abs(seedPos.y - (vi.y + j)), voxelResolution.y);
				for (int k = 0; k < 2; ++k)
				{
					float weight3 = 1 - min(abs(seedPos.z - (vi.z + k)), voxelResolution.z);
					value += weight1 * weight2 * weight3 * ((float)voxelBuff[VoxelToArray(vi + uint3(i, j, k))].myVoxelState - VOXEL_FILLED);
				}
			}
		}

		v = value;
	}

	return v;
}

float GetNoise(float3 pos);
float3 GetBitangentNoise(float3 pos);

float GetDensity(float3 pos)
{
	//float3 biTNoise = GetBitangentNoise(pos);
	//pos += biTNoise * biTagentNoiseStrength;

	float3 vp = pos - smokePos;
	float3 radius = (smokeRadius * EaseFlatten(smokeTimer, RayMarchGrowthSteepness)) - 0.1f;

	float v = getTrilinearVoxel(pos);

	float n = GetNoise(pos);

	float dist = min(1.0f, length(vp / radius));
	float voxelDist = min(1.0f, 1 - (v / 16.0f));
	dist = max(dist, voxelDist);

	dist = smoothstep(densityFalloff, 1.0f, dist);

	float falloff = min(1.0f, dist + n);

	return saturate(saturate(v) * (1 - falloff));
}

float phase(float g, float cos_theta)
{
	float p = 0.0f;

	if (phaseFunc == 0)
		p = HenyeyGreenstein(g, cos_theta);
	if (phaseFunc == 1)
		p = Mie(g, cos_theta);
	if (phaseFunc == 2)
		p = Rayleigh(cos_theta);

	return p;
}

#define MAX_DIST 200

// https://www.shadertoy.com/view/ftKfzc
float InterleavedGradientNoise(vec2 uv, int FrameId) {
	// magic values are found by experimentation
	uv += float(FrameId) * (vec2(47, 17) * 0.695f);

	vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);

	//https://juejin.cn/post/6844903687505068045
	//vec3 magic = vec3( 12.9898, 78.233, 43758.5453123 );

	return fract(magic.z * fract(dot(uv, magic.xy)));
}

uvec2 _pcg3d16(uvec3 p)
{
	uvec3 v = p * 1664525u + 1013904223u;
	v.x += v.y * v.z; v.y += v.z * v.x; v.z += v.x * v.y;
	v.x += v.y * v.z; v.y += v.z * v.x;
	return v.xy;
}
uvec2 _pcg4d16(uvec4 p)
{
	uvec4 v = p * 1664525u + 1013904223u;
	v.x += v.y * v.w; v.y += v.z * v.x; v.z += v.x * v.y; v.w += v.y * v.z;
	v.x += v.y * v.w; v.y += v.z * v.x;
	return v.xy;
}

// Get random gradient from hash value.
vec3 _gradient3d(uint hash)
{
	vec3 g = vec3(uvec3(hash, hash, hash) & uvec3(0x80000, 0x40000, 0x20000));
	return g * (1.0 / vec3(0x40000, 0x20000, 0x10000)) - 1.0;
}
vec4 _gradient4d(uint hash)
{
	vec4 g = vec4(uvec4(hash, hash, hash, hash) & uvec4(0x80000, 0x40000, 0x20000, 0x10000));
	return g * (1.0 / vec4(0x40000, 0x20000, 0x10000, 0x8000)) - 1.0;
}

// Optimized 3D Bitangent Noise. Approximately 113 instruction slots used.
// Assume p is in the range [-32768, 32767].
vec3 BitangentNoise3D(vec3 p)
{
	const vec2 C = vec2(1.0 / 6.0, 1.0 / 3.0);
	const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);

	// First corner
	vec3 i = floor(p + dot(p, C.yyy));
	vec3 x0 = p - i + dot(i, C.xxx);

	// Other corners
	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min(g.xyz, l.zxy);
	vec3 i2 = max(g.xyz, l.zxy);

	// x0 = x0 - 0.0 + 0.0 * C.xxx;
	// x1 = x0 - i1  + 1.0 * C.xxx;
	// x2 = x0 - i2  + 2.0 * C.xxx;
	// x3 = x0 - 1.0 + 3.0 * C.xxx;
	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
	vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

	i = i + 32768.5;
	uvec2 hash0 = _pcg3d16(uvec3(i));
	uvec2 hash1 = _pcg3d16(uvec3(i + i1));
	uvec2 hash2 = _pcg3d16(uvec3(i + i2));
	uvec2 hash3 = _pcg3d16(uvec3(i + 1.0));

	vec3 p00 = _gradient3d(hash0.x); vec3 p01 = _gradient3d(hash0.y);
	vec3 p10 = _gradient3d(hash1.x); vec3 p11 = _gradient3d(hash1.y);
	vec3 p20 = _gradient3d(hash2.x); vec3 p21 = _gradient3d(hash2.y);
	vec3 p30 = _gradient3d(hash3.x); vec3 p31 = _gradient3d(hash3.y);

	// Calculate noise gradients.
	vec4 m = clamp(0.5 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0, 1.0);
	vec4 mt = m * m;
	vec4 m4 = mt * mt;

	mt = mt * m;
	vec4 pdotx = vec4(dot(p00, x0), dot(p10, x1), dot(p20, x2), dot(p30, x3));
	vec4 temp = mt * pdotx;
	vec3 gradient0 = -8.0 * (temp.x * x0 + temp.y * x1 + temp.z * x2 + temp.w * x3);
	gradient0 += m4.x * p00 + m4.y * p10 + m4.z * p20 + m4.w * p30;

	pdotx = vec4(dot(p01, x0), dot(p11, x1), dot(p21, x2), dot(p31, x3));
	temp = mt * pdotx;
	vec3 gradient1 = -8.0 * (temp.x * x0 + temp.y * x1 + temp.z * x2 + temp.w * x3);
	gradient1 += m4.x * p01 + m4.y * p11 + m4.z * p21 + m4.w * p31;

	// The cross products of two gradients is divergence free.
	return cross(gradient0, gradient1) * 3918.76;
}

// 4D Bitangent noise. Approximately 163 instruction slots used.
// Assume p is in the range [-32768, 32767].
vec3 BitangentNoise4D(vec4 p)
{
	const vec4 F4 = vec4(0.309016994374947451, 0.309016994374947451, 0.309016994374947451, 0.309016994374947451);
	const vec4  C = vec4(0.138196601125011,  // (5 - sqrt(5))/20  G4
		0.276393202250021,  // 2 * G4
		0.414589803375032,  // 3 * G4
		-0.447213595499958); // -1 + 4 * G4

	// First corner
	vec4 i = floor(p + dot(p, F4));
	vec4 x0 = p - i + dot(i, C.xxxx);

	// Other corners

	// Rank sorting originally contributed by Bill Licea-Kane, AMD (formerly ATI)
	vec4 i0;
	vec3 isX = step(x0.yzw, x0.xxx);
	vec3 isYZ = step(x0.zww, x0.yyz);
	// i0.x = dot( isX, vec3( 1.0 ) );
	i0.x = isX.x + isX.y + isX.z;
	i0.yzw = 1.0 - isX;
	// i0.y += dot( isYZ.xy, vec2( 1.0 ) );
	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0 - isYZ.xy;
	i0.z += isYZ.z;
	i0.w += 1.0 - isYZ.z;

	// i0 now contains the unique values 0,1,2,3 in each channel
	vec4 i3 = clamp(i0, 0.0, 1.0);
	vec4 i2 = clamp(i0 - 1.0, 0.0, 1.0);
	vec4 i1 = clamp(i0 - 2.0, 0.0, 1.0);

	// x0 = x0 - 0.0 + 0.0 * C.xxxx
	// x1 = x0 - i1  + 1.0 * C.xxxx
	// x2 = x0 - i2  + 2.0 * C.xxxx
	// x3 = x0 - i3  + 3.0 * C.xxxx
	// x4 = x0 - 1.0 + 4.0 * C.xxxx
	vec4 x1 = x0 - i1 + C.xxxx;
	vec4 x2 = x0 - i2 + C.yyyy;
	vec4 x3 = x0 - i3 + C.zzzz;
	vec4 x4 = x0 + C.wwww;

	i = i + 32768.5;
	uvec2 hash0 = _pcg4d16(uvec4(i));
	uvec2 hash1 = _pcg4d16(uvec4(i + i1));
	uvec2 hash2 = _pcg4d16(uvec4(i + i2));
	uvec2 hash3 = _pcg4d16(uvec4(i + i3));
	uvec2 hash4 = _pcg4d16(uvec4(i + 1.0));

	vec4 p00 = _gradient4d(hash0.x); vec4 p01 = _gradient4d(hash0.y);
	vec4 p10 = _gradient4d(hash1.x); vec4 p11 = _gradient4d(hash1.y);
	vec4 p20 = _gradient4d(hash2.x); vec4 p21 = _gradient4d(hash2.y);
	vec4 p30 = _gradient4d(hash3.x); vec4 p31 = _gradient4d(hash3.y);
	vec4 p40 = _gradient4d(hash4.x); vec4 p41 = _gradient4d(hash4.y);

	// Calculate noise gradients.
	vec3 m0 = clamp(0.6 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0, 1.0);
	vec2 m1 = clamp(0.6 - vec2(dot(x3, x3), dot(x4, x4)), 0.0, 1.0);
	vec3 m02 = m0 * m0; vec3 m03 = m02 * m0;
	vec2 m12 = m1 * m1; vec2 m13 = m12 * m1;

	vec3 temp0 = m02 * vec3(dot(p00, x0), dot(p10, x1), dot(p20, x2));
	vec2 temp1 = m12 * vec2(dot(p30, x3), dot(p40, x4));
	vec4 grad0 = -6.0 * (temp0.x * x0 + temp0.y * x1 + temp0.z * x2 + temp1.x * x3 + temp1.y * x4);
	grad0 += m03.x * p00 + m03.y * p10 + m03.z * p20 + m13.x * p30 + m13.y * p40;

	temp0 = m02 * vec3(dot(p01, x0), dot(p11, x1), dot(p21, x2));
	temp1 = m12 * vec2(dot(p31, x3), dot(p41, x4));
	vec4 grad1 = -6.0 * (temp0.x * x0 + temp0.y * x1 + temp0.z * x2 + temp1.x * x3 + temp1.y * x4);
	grad1 += m03.x * p01 + m03.y * p11 + m03.z * p21 + m13.x * p31 + m13.y * p41;

	// The cross products of two gradients is divergence free.
	return cross(grad0.xyz, grad1.xyz) * 81.0;
}

float3 GetBitangentNoise(float3 pos)
{
    // Scale and animate the position for better noise variation
    float3 scaledPos = pos * biTagentNoiseScale + animDirection * time;
    return BitangentNoise3D(scaledPos);
}

float GetNoise(float3 pos)
{
	float3 uvw = pos / smokeSize;
	uvw += animDirection * time;
	float baseNoise = noiseTex.SampleLevel(LinearRepeatSampler, uvw, 0).r;

	//// Add bitangent noise influence
	//float3 bitangentNoise = GetBitangentNoise(pos);
	//float bitangentInfluence = length(bitangentNoise) * biTagentNoiseStrength;

	return baseNoise;
}

// This Compute Shader Generates an Albedo & a Mask
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	float2 uv = 0;
	switch (smokeRes)
	{
	case 0: // full
		uv = (id.xy / float2(backBufferSize.x, backBufferSize.y)); /*current Pixel*/
		break;
	case 1: // half
		uv = (id.xy / float2(backBufferSize.x / 2, backBufferSize.y / 2)); /*current Pixel*/
		break;
	case 2: // quarter
		uv = (id.xy / float2(backBufferSize.x / 4, backBufferSize.y / 4)); /*current Pixel*/
		break;
	}
	float sceneDepth = smokeDepthTex.SampleLevel(PointClampSampler, uv, 0).r; /*the depth texture of the scene*/
	uv.y = 1.f - uv.y;
	// my UV's seem flipped and the scene depth seems to fucking flip upside down if i read it "correctly"; no idea what that would be though.
	// why does unity use HLSL but follow GLSL standards. so cursed.


#ifdef Debug_DebugTexs
	smokeAlbedoTex[id.xy] = float4(1.f, 1.f, 1.f, 1.0f);
	debug1Tex[id.xy] = float4(1.f, 0.f, 0.f, 1.0f);
	debug2Tex[id.xy] = float4(0.f, 1.f, 0.f, 1.0f);
	debug3Tex[id.xy] = float4(0.f, 0.f, 1.f, 1.0f);
#endif

#ifdef Debug_UV_SCENE_DEPTH
	smokeAlbedoTex[id.xy] = float4(uv, 0.f, 1.f);
	smokeMaskTex[id.xy] = float(sceneDepth);
	debug1Tex[id.xy] = float4(sceneDepth, 0.f, 0.f, 1.f);
	//debug2Tex[id.xy] = float4(flippedSceneDepth, 0.f, 0.f, 1.f);
	return;
#endif

	float3 worldSpacePos = GetClipToWorldPos(uv, sceneDepth);
#ifdef Debug_WORLDSPACEPOS
	smokeAlbedoTex[id.xy] = float4(worldSpacePos, 1.f);
	return;
#endif

	float3 rayOrigin = cameraPos;

	float4 calc = float4(uv * 2 - 1, 0.f, 1.f); // this is identical to how unity has it

	float3 rayDir = mul(Inverse(projectionMatrix), calc).xyz;  // Fix 1: Matrix first
	rayDir = mul(cameraMatrix, float4(rayDir, 0.0f)).xyz;      // Fix 2: Match Unity's order
	rayDir = normalize(rayDir);								   // Fix 3: Add normalization

	//#define Debug_RayDir
#ifdef Debug_RayDir
	smokeAlbedoTex[id.xy] = float4(rayDir, 1.f);
	debug1Tex[id.xy] = float4(rayDir, 1.0f);
	debug2Tex[id.xy] = float4(rayDir * 0.5 + 0.5, 1.0f);;
	debug3Tex[id.xy] = float4(rayDir.x, rayDir.y * -1, rayDir.z, 1.0f);

	//debug1Tex[id.xy] = float4(calc.xyz, 1.0f);
	//debug2Tex[id.xy] = float4(rayDir, 1.0f);
	//debug3Tex[id.xy] = float4(calc.x, calc.y * -1, calc.z, 1.0f);
	return;
#endif

	float distTraveled = 0.4f * (InterleavedGradientNoise(id.xy, smokeTimer) * InterleavedGradientNoiseStrength);
	float3 samplePos = rayOrigin + distTraveled * rayDir;
	float v = momoGetVoxel(samplePos);

	//#define Debug_v
#ifdef Debug_v
	smokeAlbedoTex[id.xy] = float4(samplePos, 1.f); // just so that i can see shit
	//debug1Tex[id.xy] = float4(v, 0.f, 0.f, 1.0f);
	debug2Tex[id.xy] = float4(momoGetVoxel(rayOrigin), 0.f, 0.f, 1.0f);
	return;
#endif


	while (v == 0 && distTraveled < MAX_DIST) // this is where we actaully raymarch
	{
		distTraveled += 0.4/* * InterleavedGradientNoise(id.xy, 0)*/; // adding this crashed my gpu lol
		samplePos = rayOrigin + distTraveled * rayDir;
		v = momoGetVoxel(samplePos);
	}

	//#define Debug_V2
#ifdef Debug_v 
	smokeAlbedoTex[id.xy] = float4(samplePos, 1.f); // just so that i can see shit
	debug3Tex[id.xy] = float4(samplePos, 1.f); // just so that i can see shit
	debug2Tex[id.xy] = float4(v, 0.f, 0.f, 1.f); // just so that i can see shit
	return;
#endif

	float3 col = 0.f;
	float alpha = 1.f;

	// prob shouldn't need to check if a voxel is occupied since we have the mask & check against the depth but idk.
	if (v && v != VOXEL_OCCUPIED) // we found a voxel, cool
	{
#ifdef Debug_V2
		smokeMaskTex[id.xy] = 0.f;
		return;
#endif

		// go back one step to avoid missing details
		distTraveled -= 0.4f;
		float thickness = 0.f;

		col = smokeColor;

		// absorption (light absorbed by the medium)
		// scattering (light redirected by particles in the medium)
		// extinction (how much light is "lost" as it travels through the smoke)
		// comes from Beer-Lambert Law
		float extinctionCoefficient = absorptionCoefficient + scatteringCoefficient;

		float cos_theta = dot(rayDir, float3(0, 1, 0));
		float p = phase(_scatteringAnisotropy, cos_theta);

		float stepSize = _stepSize; // bad naming standard when things that come from cbuffer aren't clearly labeled.
		float lStepSize = lightStepSize;
		float accumulatedDensity = 0;

		int stepCount = 0;

		// Solve for t in P = O + tD for scene depth
		//float sceneIntersectDistance = -((rayOrigin - worldSpacePos) / rayDir).x;

		//float sceneIntersectDistance = length((rayOrigin - worldSpacePos)); // this is what björn suggested- it works but is "wrong"; since it doesn't calculate t anymore.

		float3 diff = worldSpacePos - rayOrigin;  // vector from rayOrigin to worldSpacePos
		float denom = dot(rayDir, normalize(diff));  // projection along rayDir
		float sceneIntersectDistance = (abs(denom) > 1e-6) ? max(length(diff) / denom, 0.0) : FLT_MAX;

		// main volumeetric light pass
		while ((stepCount < _stepCount) && distTraveled < sceneIntersectDistance)
		{
			samplePos = rayOrigin + distTraveled * rayDir;

			v = GetDensity(samplePos) /** bulletHole(samplePos, getNoise(samplePos))*/;
			float sampleDensity = v;
			accumulatedDensity += sampleDensity * volumeDensity;

			++stepCount;
			distTraveled += stepSize;
			thickness += stepSize * sampleDensity;
			alpha = exp(-thickness * accumulatedDensity * extinctionCoefficient);
			if (v > 0.001f)
			{
				float tau = 0.0f;
				float3 lightPos = samplePos;
				int lStepCount = 0;
				while (lStepCount < lightStepCount)
				{
					tau += v * shadowDensity;
					lightPos -= lStepSize * float3(0, -1, 0);
					v = GetDensity(lightPos);
					lStepCount++;
				}

				float3 lightAttenuation = exp(-(tau / extinctionColor) * extinctionCoefficient * shadowDensity);
				col += sunColor * lightAttenuation * alpha * p * scatteringCoefficient * volumeDensity * sampleDensity;
			}

			if (alpha < alphaThreshold)
				break;
		}

		// inbetween sample in the case of overshooting scene depth
		if (distTraveled > sceneIntersectDistance)
		{
			samplePos -= (distTraveled - sceneIntersectDistance) * rayDir;
			thickness -= distTraveled - sceneIntersectDistance;

			v = GetDensity(samplePos);
			float sampleDensity = v;
			accumulatedDensity += volumeDensity * sampleDensity;
			alpha = exp(-thickness * accumulatedDensity * extinctionCoefficient);
			if (v > 0.001f)
			{
				float tau = 0.0f;
				float3 lightPos = samplePos;
				int lStepCount = 0;
				while (lStepCount < lightStepCount)
				{
					tau += v * shadowDensity;
					lightPos -= lStepSize * float3(0, -1, 0);
					v = GetDensity(lightPos);
					lStepCount++;
				}

				float3 lightAttenuation = exp(-(tau / extinctionColor) * extinctionCoefficient * shadowDensity);
				col += sunColor * lightAttenuation * alpha * p * scatteringCoefficient * volumeDensity * sampleDensity;
			}
		}

		if (alpha < alphaThreshold)
			alpha = 0;
	}

	smokeAlbedoTex[id.xy] = float4(saturate(col), 1.f);
	smokeMaskTex[id.xy] = saturate(alpha);

	const float2 debugCol = float2(0.f, 0.f);
	debug1Tex[id.xy] = float4(GetDensity(samplePos), debugCol.xy, 1.f);
	debug2Tex[id.xy] = float4(getTrilinearVoxel(samplePos), debugCol.xy, 1.f);
	debug3Tex[id.xy] = float4(GetNoise(samplePos) * 2.5, debugCol.xy, 1.f); // mult to get stronger results
}