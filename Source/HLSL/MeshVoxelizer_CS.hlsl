#include "PortfolioCommon.hlsli"
#include "Common.hlsli"

struct Vertex
{
	float4 position;
	float4 color;
	float2 uv;
	float3 normal;
	float3 tangent;
	float3 biTangent; // or binormal
};

struct MeshTri
{
	Vertex vertices[3];
};

RWStructuredBuffer<VoxelInstance> voxelBuff : register(u0); // destination
StructuredBuffer<MeshTri> meshTris : register(t0); // source

#pragma region COLLISION
//
// https://bronsonzgeb.com/index.php/2021/05/29/gpu-mesh-voxelizer-part-2/
//

bool IntersectsTriangleAabbSat(float3 v0, float3 v1, float3 v2, float3 aabbExtents, float3 axis)
{
	float p0 = dot(v0, axis);
	float p1 = dot(v1, axis);
	float p2 = dot(v2, axis);

	float r = aabbExtents.x * abs(dot(float3(1, 0, 0), axis)) +
		aabbExtents.y * abs(dot(float3(0, 1, 0), axis)) +
		aabbExtents.z * abs(dot(float3(0, 0, 1), axis));

	float maxP = max(p0, max(p1, p2));
	float minP = min(p0, min(p1, p2));

	return !(max(-maxP, minP) > r);
}

struct colTri
{
	float3 a;
	float3 b;
	float3 c;
};

struct AABB
{
	float3 center;
	float3 extents;
};

bool IntersectsTriangleAabb(colTri tri, AABB aabb)
{
	tri.a -= aabb.center;
	tri.b -= aabb.center;
	tri.c -= aabb.center;

	float3 ab = normalize(tri.b - tri.a);
	float3 bc = normalize(tri.c - tri.b);
	float3 ca = normalize(tri.a - tri.c);

	//Cross ab, bc, and ca with (1, 0, 0)
	float3 a00 = float3(0.0, -ab.z, ab.y);
	float3 a01 = float3(0.0, -bc.z, bc.y);
	float3 a02 = float3(0.0, -ca.z, ca.y);

	//Cross ab, bc, and ca with (0, 1, 0)
	float3 a10 = float3(ab.z, 0.0, -ab.x);
	float3 a11 = float3(bc.z, 0.0, -bc.x);
	float3 a12 = float3(ca.z, 0.0, -ca.x);

	//Cross ab, bc, and ca with (0, 0, 1)
	float3 a20 = float3(-ab.y, ab.x, 0.0);
	float3 a21 = float3(-bc.y, bc.x, 0.0);
	float3 a22 = float3(-ca.y, ca.x, 0.0);

	if (
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a00) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a01) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a02) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a10) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a11) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a12) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a20) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a21) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, a22) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, float3(1, 0, 0)) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, float3(0, 1, 0)) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, float3(0, 0, 1)) ||
		!IntersectsTriangleAabbSat(tri.a, tri.b, tri.c, aabb.extents, cross(ab, bc))
		)
	{
		return false;
	}

	return true;
}

#pragma endregion

[numthreads(128, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	//// position in voxel space
    uint3 voxelPos = ArrayToVoxel(id.x);

	// Load triangle data

    float halfVoxelSize = voxelSize * .5f;
    float3 centerPos = voxelPos * voxelSize + halfVoxelSize; // world pos
    centerPos.xz -= boundingBoxExtents.xz;

    //float intersectionBias = .9999999f; // this is useless and breaks everything, shit
    float intersectionBias = 1.f; // this is useless and breaks everything, shit
    AABB aabb;
    aabb.center = centerPos; // at 0,0,0 this number is -14.75, 0.25, -14.75 rn
    aabb.extents = halfVoxelSize * intersectionBias; // and this is 0.25

    bool intersects = false;
    for (int i = 0; i < meshesToBeVoxelizedTriCount; i++)
    {
        colTri tri;
        tri.a = meshTris[i].vertices[0].position;
        tri.b = meshTris[i].vertices[1].position;
        tri.c = meshTris[i].vertices[2].position;

        intersects = IntersectsTriangleAabb(tri, aabb);

        if (intersects)
            break;
    }
	// https://bronsonzgeb.com/index.php/2021/05/29/gpu-mesh-voxelizer-part-2/
	// https://github.com/GarrettGunnell/CS2-Smoke-Grenades/blob/main/Assets/Resources/Voxelize.compute
    voxelBuff[id.x].myVoxelState = intersects ? VOXEL_OCCUPIED : VOXEL_EMPTY;
}