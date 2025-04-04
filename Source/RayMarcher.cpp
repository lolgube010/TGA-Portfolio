#include "RayMarcher.h"
#include <d3d11.h>
#include <fstream>

#include "GraphicsEngine.h"
#include "PortfolioSettings.h"

void RayMarcher::Render(ID3D11DeviceContext* aDeviceContext, RayMarcherStructs::DepthBuffer& aDepthBuffer, const ComPtr<ID3D11ShaderResourceView>& aVoxelBufferSRV, ComPtr<ID3D11RenderTargetView>& aBackBufferRTV, const ComPtr<ID3D11Texture2D>& aBackBufferTex, ComPtr<ID3D11UnorderedAccessView>& aVoxelBufferUAV, ID3D11Device
	* aDevice)
{
	if (PortfolioSettings::updateNoise)
	{
		GenerateNoise(aDeviceContext);
	}

	aDeviceContext->OMSetRenderTargets(1, depthTex.rtv.GetAddressOf(), nullptr); // bind tex as resource
	aDeviceContext->PSSetShaderResources(1, 1, aDepthBuffer.SRV.GetAddressOf()); // bind our main depthBuffer as resource
	// zlip true, backcull, lessEqual, zWrite on
	mySampleDepthPass.Render(aDeviceContext);

	UnbindRTV(aDeviceContext, 1);
	PSUnbindSRV(aDeviceContext, 1, 1);

	auto& smokeAlbedoTex = GetSmokeAlbedoTex();
	auto& smokeMaskTex = GetSmokeMaskTex();

	//if (PortfolioSettings::debugNoise)
	//{
	//	// run debug noise cs here
	//	// dispatches debug compute at
	//	// mathf.ceiltoInt(screenWidth / 8), mathfceilToInt(screenHeight / 8.f), 1.f);
	//	// blit smokeTex to destination
	//}

	aDeviceContext->CSSetShader(myRenderSmokeCS.Get(), nullptr, 0);

	// bind UAV
	// slot 0 should be the voxelBuffer UAV
	aDeviceContext->CSSetUnorderedAccessViews(1, 1, smokeAlbedoTex.uav.GetAddressOf(), nullptr);
	aDeviceContext->CSSetUnorderedAccessViews(2, 1, smokeMaskTex.uav.GetAddressOf(), nullptr);

	aDeviceContext->CSSetUnorderedAccessViews(5, 1, debugTex1.uav.GetAddressOf(), nullptr);
	aDeviceContext->CSSetUnorderedAccessViews(6, 1, debugTex2.uav.GetAddressOf(), nullptr);
	aDeviceContext->CSSetUnorderedAccessViews(7, 1, debugTex3.uav.GetAddressOf(), nullptr);

	// TODO, DEBUG CODE UNDO!
	PSUnbindSRV(aDeviceContext, 0, 1);
	VSUnbindSRV(aDeviceContext, 0, 1);
	aDeviceContext->CSSetUnorderedAccessViews(4, 1, aVoxelBufferUAV.GetAddressOf(), nullptr);

	// bind SRV
	aDeviceContext->CSSetShaderResources(1, 1, depthTex.srv.GetAddressOf());
	aDeviceContext->CSSetShaderResources(2, 1, noiseTex.srv.GetAddressOf());
	aDeviceContext->CSSetShaderResources(3, 1, curlTex.srv.GetAddressOf());
	//aDeviceContext->CSSetShaderResources(5, 1, aVoxelBufferSRV.GetAddressOf());
	// TODO- UNCOMMENT ABOVE!
	aVoxelBufferSRV;

	// dispatch renderSmokeCS
	aDeviceContext->Dispatch(static_cast<int>(std::ceilf(smokeAlbedoTex.width / 8.f)), static_cast<int>(std::ceilf(smokeAlbedoTex.height / 8.f)), 1);

	// unbind
	CSUnbindUAV(aDeviceContext, 1, 2); // unbind smoke and albedo UAV
	CSUnbindUAV(aDeviceContext, 5, 3); // unbind debug textures
	CSUnbindSRV(aDeviceContext, 1, 3); // unbind depth AND noise, depth is re-used but only in PS from now on
	CSUnbindSRV(aDeviceContext, 5, 1); // unbind voxel buffer

	// TODO- UNDO!
	CSUnbindUAV(aDeviceContext, 4, 1); // unbind debug voxel buffer 

	// if using other resolution scales than full, downscale here to increase FPS!
#pragma region downscaleHalf
	if (PortfolioSettings::resScale == RayMarcherStructs::Res::Half)
	{
		// mask: half --> full
		aDeviceContext->PSSetShaderResources(5, 1, smokeMaskHalfTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeMaskFullTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);

		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(smokeMaskHalfTex.width);  // e.g., 512
		viewport.Height = static_cast<float>(smokeMaskHalfTex.height); // e.g., 512
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		aDeviceContext->RSSetViewports(1, &viewport);

		// mask: full --> half
		aDeviceContext->PSSetShaderResources(5, 1, smokeMaskFullTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeMaskHalfTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);

		viewport.Width = static_cast<float>(smokeMaskFullTex.width);
		viewport.Height = static_cast<float>(smokeMaskFullTex.height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		aDeviceContext->RSSetViewports(1, &viewport);

		// albedo: half --> full
		aDeviceContext->PSSetShaderResources(5, 1, smokeAlbedoHalfTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeAlbedoFullTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);
	}
#pragma endregion

#pragma region downscaleQuarter
	if (PortfolioSettings::resScale == RayMarcherStructs::Res::Quarter)
	{
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(smokeMaskHalfTex.width);  // e.g., 512
		viewport.Height = static_cast<float>(smokeMaskHalfTex.height); // e.g., 512
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		aDeviceContext->RSSetViewports(1, &viewport);

		// mask: quarter --> half
		aDeviceContext->PSSetShaderResources(5, 1, smokeMaskQuarterTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeMaskHalfTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);

		viewport.Width = static_cast<float>(smokeMaskFullTex.width);  // e.g., 512
		viewport.Height = static_cast<float>(smokeMaskFullTex.height); // e.g., 512
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		aDeviceContext->RSSetViewports(1, &viewport);

		// mask: half --> full
		aDeviceContext->PSSetShaderResources(5, 1, smokeMaskHalfTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeMaskFullTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);

		viewport.Width = static_cast<float>(smokeMaskHalfTex.width);  // e.g., 512
		viewport.Height = static_cast<float>(smokeMaskHalfTex.height); // e.g., 512
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		aDeviceContext->RSSetViewports(1, &viewport);

		// mask: full --> half
		aDeviceContext->PSSetShaderResources(5, 1, smokeMaskFullTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeMaskHalfTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);

		viewport.Width = static_cast<float>(smokeMaskQuarterTex.width);  // e.g., 512
		viewport.Height = static_cast<float>(smokeMaskQuarterTex.height); // e.g., 512
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		aDeviceContext->RSSetViewports(1, &viewport);

		// mask: half --> quarter
		aDeviceContext->PSSetShaderResources(5, 1, smokeMaskHalfTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeMaskQuarterTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);

		viewport.Width = static_cast<float>(smokeAlbedoHalfTex.width);  // e.g., 512
		viewport.Height = static_cast<float>(smokeAlbedoHalfTex.height); // e.g., 512
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		aDeviceContext->RSSetViewports(1, &viewport);

		// albedo: quarter --> half
		aDeviceContext->PSSetShaderResources(5, 1, smokeAlbedoQuarterTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeAlbedoHalfTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);

		viewport.Width = static_cast<float>(smokeAlbedoFullTex.width);  // e.g., 512
		viewport.Height = static_cast<float>(smokeAlbedoFullTex.height); // e.g., 512
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		aDeviceContext->RSSetViewports(1, &viewport);

		// albedo: half --> full
		aDeviceContext->PSSetShaderResources(5, 1, smokeAlbedoHalfTex.srv.GetAddressOf());
		aDeviceContext->OMSetRenderTargets(1, smokeAlbedoFullTex.rtv.GetAddressOf(), nullptr);
		mySimpleBlit.Render(aDeviceContext);
		PSUnbindSRV(aDeviceContext, 5, 1);
		UnbindRTV(aDeviceContext, 1);
	}
#pragma endregion

	// copy main texture (bb)
	DirectX::XMUINT2 realWH = RayMarcherStructs::RenderTexture::GetTextureWidthHeight(aBackBufferTex.Get());
	if (mainTexCopy.width != realWH.x || mainTexCopy.height != realWH.y)
	{
		mainTexCopy.srv->Release();
		mainTexCopy.tex->Release();
		mainTexCopy.rtv->Release();
		CreateRenderTexture2D(mainTexCopy, realWH.x, realWH.y, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, aDevice, false);
	}
	aDeviceContext->CopyResource(mainTexCopy.tex.Get(), aBackBufferTex.Get());

	// bind smoke albedo & mask to for sharpening pass
	aDeviceContext->PSSetShaderResources(1, 1, smokeAlbedoFullTex.srv.GetAddressOf());
	aDeviceContext->PSSetShaderResources(2, 1, smokeMaskTex.srv.GetAddressOf());
	aDeviceContext->PSSetShaderResources(3, 1, depthTex.srv.GetAddressOf());
	aDeviceContext->PSSetShaderResources(4, 1, mainTexCopy.srv.GetAddressOf());

	// bind RTV as active:
	aDeviceContext->OMSetRenderTargets(1, aBackBufferRTV.GetAddressOf(), nullptr); // bind bb as target (to additively combine with)

	// DEBUG! TODO REMOVE
	//aDeviceContext->OMSetRenderTargets(1, debugTex1.rtv.GetAddressOf(), nullptr); // bind bb as target (to additively combine with)
	aBackBufferRTV;

	mySharpeningPass.Render(aDeviceContext);

	PSUnbindSRV(aDeviceContext, 1, 4); // unbind smoke albedo, mask, depth, & mainTexCopy
}

void RayMarcher::Init(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext, const UINT aBBWidth, const UINT aBBHeight)
{
	InitTextures(aDevice, aBBWidth, aBBHeight);
	InitComputeShaders(aDevice);
	InitFullscreenPasses(aDevice);
	GenerateNoise(aDeviceContext);
}

void RayMarcher::InitTextures(ID3D11Device* aDevice, const UINT aBBWidth, const UINT aBBHeight)
{
	const auto halfWidth = aBBWidth / 2; const auto halfHeight = aBBHeight / 2;
	const auto quarterWidth = aBBWidth / 4; const auto quarterHeight = aBBHeight / 4;

	// NOTE- TODO- README! PLEASE CHECK IN FUTURE! TEXTURES SIZES ARE ALWAYS ROUNDED DOWN! SO 1102x515 = 275x128!!!
	CreateRenderTexture3D(noiseTex, 128, 128, 128, DXGI_FORMAT_R16_FLOAT, aDevice);
	CreateRenderTexture3D(curlTex, 128, 128, 128, DXGI_FORMAT_R16G16B16A16_FLOAT, aDevice);
	//CreateRenderTexture2D(depthTex, aBBWidth, aBBHeight, DXGI_FORMAT_R16_FLOAT, aDevice); // WAS R16 BEFORE, NOW R32 DUE TO CLAMP ISSUES???!!!!!!!!!!!!!
	CreateRenderTexture2D(depthTex, aBBWidth, aBBHeight, DXGI_FORMAT_R32_FLOAT, aDevice);
	/*DXGI_FORMAT_R16G16B16A16_UNORM this below instead?*/
	CreateRenderTexture2D(smokeAlbedoFullTex, aBBWidth, aBBHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, aDevice);
	CreateRenderTexture2D(smokeAlbedoHalfTex, halfWidth, halfHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, aDevice);
	CreateRenderTexture2D(smokeAlbedoQuarterTex, quarterWidth, quarterHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, aDevice);

	CreateRenderTexture2D(smokeMaskFullTex, aBBWidth, aBBHeight, DXGI_FORMAT_R32_FLOAT, aDevice);
	CreateRenderTexture2D(smokeMaskHalfTex, halfWidth, halfHeight, DXGI_FORMAT_R32_FLOAT, aDevice);
	CreateRenderTexture2D(smokeMaskQuarterTex, quarterWidth, quarterHeight, DXGI_FORMAT_R32_FLOAT, aDevice);

	CreateRenderTexture2D(debugTex1, aBBWidth, aBBHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, aDevice);
	CreateRenderTexture2D(debugTex2, aBBWidth, aBBHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, aDevice);
	CreateRenderTexture2D(debugTex3, aBBWidth, aBBHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, aDevice);

	CreateRenderTexture2D(mainTexCopy, aBBWidth, aBBHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, aDevice, false);
}

void RayMarcher::InitComputeShaders(ID3D11Device* aDevice)
{
	GraphicsEngine::CreateComputeShader(aDevice, "SmokeRenderer_CS.cso", myRenderSmokeCS);
	GraphicsEngine::CreateComputeShader(aDevice, "GenerateNoise_CS.cso", myGenerateNoiseCS);
}

void RayMarcher::InitFullscreenPasses(ID3D11Device* aDevice)
{
	mySampleDepthPass.Init("SampleDepth_PS", aDevice);
	mySharpeningPass.Init("Sharpening_PS", aDevice);
	mySimpleBlit.Init("SimpleCopyPS", aDevice);
}

void RayMarcher::CreateRenderTexture2D(RayMarcherStructs::RenderTexture2D& aRenderTexture, UINT aWidth, UINT aHeight, DXGI_FORMAT aFormat, ID3D11Device* aDevice, bool aCreateUAV)
{
	aRenderTexture.width = aWidth;
	aRenderTexture.height = aHeight;

	D3D11_TEXTURE2D_DESC desc = GetTextureDesc2D(aWidth, aHeight, aFormat);
	HRESULT res = aDevice->CreateTexture2D(&desc, nullptr, aRenderTexture.tex.GetAddressOf());
	if (FAILED(res))
	{
		throw;
	}

	if (aCreateUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = desc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		res = aDevice->CreateUnorderedAccessView(aRenderTexture.tex.Get(), &uavDesc, aRenderTexture.uav.GetAddressOf());
		if (FAILED(res))
		{
			throw;
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	res = aDevice->CreateShaderResourceView(aRenderTexture.tex.Get(), &srvDesc, aRenderTexture.srv.GetAddressOf());
	if (FAILED(res))
	{
		throw;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	res = aDevice->CreateRenderTargetView(aRenderTexture.tex.Get(), &rtvDesc, aRenderTexture.rtv.GetAddressOf());
	if (FAILED(res))
	{
		throw;
	}
}

D3D11_TEXTURE2D_DESC RayMarcher::GetTextureDesc2D(const UINT aWidth, const UINT aHeight, const DXGI_FORMAT aFormat)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = aWidth;
	desc.Height = aHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = aFormat; // Or your preferred format
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	return desc;
}

void RayMarcher::CreateRenderTexture3D(RayMarcherStructs::RenderTexture3D& aRenderTexture, UINT aWidth, UINT aHeight, UINT aDepth, DXGI_FORMAT aFormat, ID3D11Device* aDevice)
{
	aRenderTexture.width = aWidth;
	aRenderTexture.height = aHeight;
	aRenderTexture.depth = aDepth;

	D3D11_TEXTURE3D_DESC desc = GetTextureDesc3D(aWidth, aHeight, aDepth, aFormat);
	HRESULT res = aDevice->CreateTexture3D(&desc, nullptr, aRenderTexture.tex.GetAddressOf());
	if (FAILED(res))
	{
		throw;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = desc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	uavDesc.Texture3D.MipSlice = 0;
	uavDesc.Texture3D.FirstWSlice = 0;
	uavDesc.Texture3D.WSize = desc.Depth;
	res = aDevice->CreateUnorderedAccessView(aRenderTexture.tex.Get(), &uavDesc, aRenderTexture.uav.GetAddressOf());
	if (FAILED(res))
	{
		throw;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MipLevels = 1;
	srvDesc.Texture3D.MostDetailedMip = 0;
	res = aDevice->CreateShaderResourceView(aRenderTexture.tex.Get(), &srvDesc, aRenderTexture.srv.GetAddressOf());
	if (FAILED(res))
	{
		throw;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
	rtvDesc.Texture3D.MipSlice = 0;
	rtvDesc.Texture3D.FirstWSlice = 0;
	rtvDesc.Texture3D.WSize = desc.Depth;
	res = aDevice->CreateRenderTargetView(aRenderTexture.tex.Get(), &rtvDesc, aRenderTexture.rtv.GetAddressOf());
	if (FAILED(res))
	{
		throw;
	}
}

D3D11_TEXTURE3D_DESC RayMarcher::GetTextureDesc3D(const UINT aWidth, const UINT aHeight, const UINT aDepth, const DXGI_FORMAT aFormat)
{
	D3D11_TEXTURE3D_DESC desc = {};
	desc.Width = aWidth;         // X dimension
	desc.Height = aHeight;        // Y dimension
	desc.Depth = aDepth;         // Z dimension (volumeDepth in Unity)
	desc.MipLevels = 1;       // Single mip level
	desc.Format = aFormat; // DXGI_FORMAT_R16_FLOAT = RHalf equivalent
	desc.Usage = D3D11_USAGE_DEFAULT;    // Required for UAV
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // UAV for compute, SRV for reading if needed
	return desc;
}

RayMarcherStructs::RenderTexture2D& RayMarcher::GetSmokeAlbedoTex()
{
	switch (PortfolioSettings::resScale)
	{
	case RayMarcherStructs::Res::Full:
		return smokeAlbedoFullTex;
	case RayMarcherStructs::Res::Half:
		return smokeAlbedoHalfTex;
	case RayMarcherStructs::Res::Quarter:
		return smokeAlbedoQuarterTex;
	}
	throw;
}

RayMarcherStructs::RenderTexture2D& RayMarcher::GetSmokeMaskTex()
{
	switch (PortfolioSettings::resScale)
	{
	case RayMarcherStructs::Res::Full:
		return smokeMaskFullTex;
	case RayMarcherStructs::Res::Half:
		return smokeMaskHalfTex;
	case RayMarcherStructs::Res::Quarter:
		return smokeMaskQuarterTex;
	}
	throw;
}

void RayMarcher::GenerateNoise(ID3D11DeviceContext* aDeviceContext)
{
	aDeviceContext->CSSetShader(myGenerateNoiseCS.Get(), nullptr, 0);

	// bind UAV
	// slot 0 should be the voxelBuffer UAV
	aDeviceContext->CSSetUnorderedAccessViews(0, 1, noiseTex.uav.GetAddressOf(), nullptr);
	aDeviceContext->CSSetUnorderedAccessViews(1, 1, curlTex.uav.GetAddressOf(), nullptr);

	// dispatch renderSmokeCS
	aDeviceContext->Dispatch(16, 16, 16);

	// unbind
	CSUnbindUAV(aDeviceContext, 0, 2);
}

void RayMarcher::UnbindRTV(ID3D11DeviceContext* aDeviceContext, const int aNumViews)
{
	static const std::vector<ID3D11RenderTargetView*> NULL_RTV(aNumViews, nullptr);
	aDeviceContext->OMSetRenderTargets(aNumViews, NULL_RTV.data(), nullptr);
}

void RayMarcher::CSUnbindSRV(ID3D11DeviceContext* aDeviceContext, const int aStartSlot, const int aNumViews)
{
	const std::vector<ID3D11ShaderResourceView*> NULL_SRV(aNumViews, nullptr);
	aDeviceContext->CSSetShaderResources(aStartSlot, aNumViews, NULL_SRV.data());
}

void RayMarcher::PSUnbindSRV(ID3D11DeviceContext* aDeviceContext, int aStartSlot, int aNumViews)
{
	const std::vector<ID3D11ShaderResourceView*> NULL_SRV(aNumViews, nullptr);
	aDeviceContext->PSSetShaderResources(aStartSlot, aNumViews, NULL_SRV.data());
}

void RayMarcher::VSUnbindSRV(ID3D11DeviceContext* aDeviceContext, int aStartSlot, int aNumViews)
{
	const std::vector<ID3D11ShaderResourceView*> NULL_SRV(aNumViews, nullptr);
	aDeviceContext->VSSetShaderResources(aStartSlot, aNumViews, NULL_SRV.data());
}

void RayMarcher::CSUnbindUAV(ID3D11DeviceContext* aDeviceContext, const int aStartSlot, const int aNumViews)
{
	const std::vector<ID3D11UnorderedAccessView*> NULL_UAV(aNumViews, nullptr);
	aDeviceContext->CSSetUnorderedAccessViews(aStartSlot, aNumViews, NULL_UAV.data(), nullptr);
}

void RayMarcher::SetDebugObjectName(ID3D11DeviceChild* aResource, const std::string& aDebugName)
{
	if (const HRESULT res = aResource->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(aDebugName), aDebugName.c_str()); FAILED(res))
	{
		throw;
	}
}