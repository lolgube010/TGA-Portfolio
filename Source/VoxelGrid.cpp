#include "VoxelGrid.h"

#include "GraphicsEngine.h"
#include "MomoMath.h"
#include "PortfolioSettings.h"

void momo_portfolio::VoxelGrid::Init(const ComPtr<ID3D11Device>& aDevice, const ComPtr<ID3D11InputLayout>& aInputLayout)
{
	//myBB.Extents = myBBExtents; // 15, 2.5, 15

	auto boundingBoxFullPerimeter = MomoMath::MultiplyXMFLOAT3(myBBExtents, 2.f); // 30, 5, 30
	//myVoxelGridRes = MomoMath::MultiplyXMFLOAT3(boundingBoxFullPerimeter, 1 / PortfolioSettings::voxelSize); // 60, 10, 60, same as dividing by the voxel size we want
	// 60, 10, 60
	myVoxelGridRes.x = static_cast<int>(std::ceilf(boundingBoxFullPerimeter.x / PortfolioSettings::voxelSize));
	myVoxelGridRes.y = static_cast<int>(std::ceilf(boundingBoxFullPerimeter.y / PortfolioSettings::voxelSize));
	myVoxelGridRes.z = static_cast<int>(std::ceilf(boundingBoxFullPerimeter.z / PortfolioSettings::voxelSize));

	// create compute buffer with size of myvoxelGrix x * y * z (36000 elements) and send to gpu
	voxelData = { GetElementCount(), { 0, {0,0,0} } };

	GraphicsEngine::CreatePixelShader(aDevice.Get(), "VoxelGrid_PS", myVoxelGridPS);
	GraphicsEngine::CreateVertexShaderAndMaybeInputLayout(aDevice.Get(), "VoxelGrid_VS", myVoxelGridVS);

	myInstancedVoxelCube.Init(aDevice.Get(), myVoxelGridPS, myVoxelGridVS, aInputLayout);
	myInstancedVoxelCube.SetPosition({ 0, 0, 0 });
}

Mesh* momo_portfolio::VoxelGrid::GetVoxelMesh()
{
	return &myInstancedVoxelCube;
}

unsigned int momo_portfolio::VoxelGrid::GetElementCount() const
{
	return (myVoxelGridRes.x * myVoxelGridRes.y * myVoxelGridRes.z);
}

//size_t momo_portfolio::VoxelGrid::WorldSpaceToArrayIndex(DirectX::XMFLOAT3 aPos) const
//{
//	return aPos.x + aPos.y * myVoxelGridRes.x + aPos.z * (myVoxelGridRes.x * myVoxelGridRes.y);
//}
//
//DirectX::XMFLOAT3 momo_portfolio::VoxelGrid::ArrayIndexToWorldSpacePos(const size_t aIndex) const
//{
//	DirectX::XMFLOAT3 indexPos = { ArrayIndexToPosXVoxelSpace(aIndex), ArrayIndexToPosXVoxelSpace(aIndex), ArrayIndexToPosXVoxelSpace(aIndex) };
//
//	//to get world space, we multiply our position by our voxel size
//	indexPos = MomoMath::MultiplyXMFLOAT3(indexPos, PortfolioSettings::voxelSize);
//
//	// and subtract the bounds extent of the grid.
//	indexPos = MomoMath::SubtractXMFLOAT3(indexPos, myBBExtents);
//
//	return indexPos;
//}
//// x = i mod 60
//float momo_portfolio::VoxelGrid::ArrayIndexToPosXVoxelSpace(const size_t aIndex) const
//{
//	return fmodf(static_cast<float>(aIndex), myVoxelGridRes.x);
//}
//// y = (i / 60) mod 10
//float momo_portfolio::VoxelGrid::ArrayIndexToPosYVoxelSpace(const size_t aIndex) const
//{
//	return fmodf((static_cast<float>(aIndex) / (myVoxelGridRes.x)), (myVoxelGridRes.y));
//}
//// z = i / (60 * 10)
//float momo_portfolio::VoxelGrid::ArrayIndexToPosZVoxelSpace(const size_t aIndex) const
//{
//	return (static_cast<float>(aIndex) / (myVoxelGridRes.x * myVoxelGridRes.y));
//}
