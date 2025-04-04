#pragma once
#include <DirectXMath.h>
#include <vector>

class Gun
{
	struct BulletHole
	{
		DirectX::XMFLOAT3 origin;
		DirectX::XMFLOAT3 forward;
		DirectX::XMFLOAT2 radius; // we'll see if we use this the same way since valve uses the texture
		float t;
		bool active;
	};
	std::vector<BulletHole> bulletHoles;
	int bulletHoleCount = 0; // current active bHoles
	int maxBulletHoles;
	// compute buffer, bulletHoleBuffer(maxBulletHoles, sizeof(GPUBulletHole)

	struct GPUBulletHole 
	{
		DirectX::XMFLOAT3 origin;
		DirectX::XMFLOAT3 forward;
		DirectX::XMFLOAT2 radius;
	};
	std::vector<GPUBulletHole> activeBulletHoles;
};
