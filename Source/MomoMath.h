#pragma once
#include <DirectXMath.h>

namespace MomoMath
{
#pragma region XM
	inline DirectX::XMFLOAT3 MultiplyXMFLOAT3(const DirectX::XMFLOAT3& vec, const float scalar)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	inline DirectX::XMFLOAT3 MultiplyXMFLOAT3(const DirectX::XMFLOAT3& vec, const DirectX::XMFLOAT3& vec2)
	{
		return { vec.x * vec2.x, vec.y * vec2.y, vec.z * vec2.z };
	}

	inline DirectX::XMFLOAT3 MultiplyXMFLOAT3(const DirectX::XMFLOAT3& vec, const CU::Vector3f& vec2)
	{
		return { vec.x * vec2.x, vec.y * vec2.y, vec.z * vec2.z };
	}

	inline DirectX::XMINT3 MultiplyXMINT3(const DirectX::XMINT3& vec, const int scalar)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	inline DirectX::XMUINT3 MultiplyXMUINT3(const DirectX::XMUINT3& vec, const unsigned int scalar)
	{
		return { vec.x * scalar, vec.y * scalar, vec.z * scalar };
	}

	inline DirectX::XMFLOAT3 AdditionXMFLOAT3(const DirectX::XMFLOAT3& vec, const DirectX::XMFLOAT3& vec2)
	{
		return { vec.x + vec2.x, vec.y + vec2.y, vec.z + vec2.z };
	}

	inline DirectX::XMFLOAT3 SubtractXMFLOAT3(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b) {
		DirectX::XMFLOAT3 result;
		result.x = a.x - b.x;
		result.y = a.y - b.y;
		result.z = a.z - b.z;
		return result;
	}

	inline DirectX::XMFLOAT3 DivideXMFLOAT3(const DirectX::XMFLOAT3 a, const DirectX::XMFLOAT3 b) {
		DirectX::XMFLOAT3 result;
		result.x = a.x / b.x;
		result.y = a.y / b.y;
		result.z = a.z / b.z;
		return result;
	}
#pragma endregion

	inline DirectX::XMFLOAT3 GetIntersectionPoint(const DirectX::XMFLOAT3& aOrigin, const DirectX::XMFLOAT3& aDirection, const float aDistance)
	{
		return AdditionXMFLOAT3(aOrigin, MultiplyXMFLOAT3(aDirection, aDistance));
	}

	inline DirectX::XMFLOAT3 GetIntersectionPoint(DirectX::FXMVECTOR aOrigin, DirectX::FXMVECTOR aDirection, const float aDistance)
	{
		DirectX::XMFLOAT3 result;
		XMStoreFloat3(&result, DirectX::XMVectorAdd(aOrigin, DirectX::XMVectorScale(aDirection, aDistance)));
		return result;
	}
}
