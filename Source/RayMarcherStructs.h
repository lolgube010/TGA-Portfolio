#pragma once
#include "DX11FwdDecl.h"
#include "DirectXMath.h"
namespace RayMarcherStructs
{
	struct DepthBuffer
	{
		ComPtr<ID3D11DepthStencilView> DSV;
		ComPtr<ID3D11ShaderResourceView> SRV;
		ComPtr<ID3D11Texture2D> Tex;
	};

	struct RenderTexture
	{
		ComPtr<ID3D11ShaderResourceView> srv = nullptr;    // todo, needed? only needed in ps (i guess so?)
		ComPtr<ID3D11UnorderedAccessView> uav = nullptr;   // Shared unordered access view
		ComPtr<ID3D11RenderTargetView> rtv = nullptr; // NOTE- YOU MIGHT WANT TO USE A UAV INSTEAD OF RTV BCZ ComputeShaders
		UINT width;
		UINT height;

		static DirectX::XMUINT2 GetTextureWidthHeight(ID3D11Texture2D* texture)
		{
			if (!texture)
				return { 0,0 };

			D3D11_TEXTURE2D_DESC desc;
			texture->GetDesc(&desc);
			return { desc.Width, desc.Height };
		}
	};

	// 2D texture version
	struct RenderTexture2D : RenderTexture
	{
		ComPtr<ID3D11Texture2D> tex = nullptr;            // 2D-specific texture
	};

	// 3D texture version
	struct RenderTexture3D : RenderTexture
	{
		ComPtr<ID3D11Texture3D> tex = nullptr;            // 3D-specific texture
		UINT depth;
	};

	enum class Res
	{
		Full,
		Half,
		Quarter
	};
	enum class DebugNoiseAxis
	{
		x, y, z
	};
	enum class PhaseFunction {
		HenyeyGreenstein,
		Mie,
		Rayleigh
	};
}
