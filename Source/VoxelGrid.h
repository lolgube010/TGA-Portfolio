#pragma once
#include <CommonUtilities/Vector3.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <cstdint>
#include <vector>
#include "Primitive.h"


namespace momo_portfolio
{
	struct Voxel // struct that the CS fills and then gets sent to the VS
	{
		unsigned int voxelState; // Empty, filled, occupied by gemoetry
		DirectX::XMFLOAT3 color;
	};

	class VoxelGrid
	{
	public:
		void Init(const ComPtr<ID3D11Device>& aDevice, const ComPtr<ID3D11InputLayout>& aInputLayout);
		Mesh* GetVoxelMesh();

		[[nodiscard]] unsigned int GetElementCount() const;
		//DirectX::BoundingBox myBB;
		DirectX::XMFLOAT3 myBBExtents = { 15, 2.5f, 15 };
		DirectX::XMINT3 myVoxelGridRes;
		std::vector<Voxel> voxelData; // maybe remove this? since most of the voxel data lives on the GPU and we just periodically update it, no need to keep a cpu copy
		Primitive<Cube> myInstancedVoxelCube;

		ComPtr<ID3D11PixelShader> myVoxelGridPS;
		ComPtr<ID3D11VertexShader> myVoxelGridVS;

	/*	size_t WorldSpaceToArrayIndex(DirectX::XMFLOAT3 aPos) const;
		DirectX::XMFLOAT3 ArrayIndexToWorldSpacePos(size_t aIndex) const;

		[[nodiscard]] inline float ArrayIndexToPosXVoxelSpace(size_t aIndex) const;
		[[nodiscard]] inline float ArrayIndexToPosYVoxelSpace(size_t aIndex) const;
		[[nodiscard]] inline float ArrayIndexToPosZVoxelSpace(size_t aIndex) const;*/
	};
}
