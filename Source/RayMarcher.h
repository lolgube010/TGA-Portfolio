#pragma once
#include <d3d11.h>
#include <string>

#include "FullscreenPass.h"
#include "RayMarcherStructs.h"

class RayMarcher
{
public:
	RayMarcherStructs::RenderTexture3D noiseTex;
	RayMarcherStructs::RenderTexture2D depthTex;
	RayMarcherStructs::RenderTexture2D smokeAlbedoFullTex, smokeAlbedoHalfTex, smokeAlbedoQuarterTex;
	RayMarcherStructs::RenderTexture2D smokeMaskFullTex, smokeMaskHalfTex, smokeMaskQuarterTex;

	RayMarcherStructs::RenderTexture2D mainTexCopy; // mainTex used for reading
	RayMarcherStructs::RenderTexture3D curlTex;
	RayMarcherStructs::RenderTexture2D debugTex1, debugTex2, debugTex3;

	ComPtr<ID3D11ComputeShader> myRenderSmokeCS;
	ComPtr<ID3D11ComputeShader> myGenerateNoiseCS;
	FullscreenPass mySampleDepthPass;
	FullscreenPass mySharpeningPass;
	FullscreenPass mySimpleBlit;

	void Render(ID3D11DeviceContext* aDeviceContext, RayMarcherStructs::DepthBuffer& aDepthBuffer, const ComPtr<ID3D11ShaderResourceView>&
	            aVoxelBufferSRV, ComPtr<ID3D11RenderTargetView>& aBackBufferRTV, const ComPtr<ID3D11Texture2D>& aBackBufferTex, ComPtr<
		            ID3D11UnorderedAccessView>& aVoxelBufferUAV, ID3D11Device* aDevice);

	void Init(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext, UINT aBBWidth, UINT aBBHeight);
	void InitTextures(ID3D11Device* aDevice, UINT aBBWidth, UINT aBBHeight);
	void InitComputeShaders(ID3D11Device* aDevice);
	void InitFullscreenPasses(ID3D11Device* aDevice);

	static void CreateRenderTexture2D(RayMarcherStructs::RenderTexture2D& aRenderTexture, UINT aWidth, UINT aHeight, DXGI_FORMAT aFormat, ID3D11Device* aDevice, bool aCreateUAV = true);
	static D3D11_TEXTURE2D_DESC GetTextureDesc2D(UINT aWidth, UINT aHeight, DXGI_FORMAT aFormat);

	static void CreateRenderTexture3D(RayMarcherStructs::RenderTexture3D& aRenderTexture, UINT aWidth, UINT aHeight, UINT aDepth, DXGI_FORMAT aFormat, ID3D11Device* aDevice);
	static D3D11_TEXTURE3D_DESC GetTextureDesc3D(UINT aWidth, UINT aHeight, UINT aDepth, DXGI_FORMAT aFormat);

	RayMarcherStructs::RenderTexture2D& GetSmokeAlbedoTex();
	RayMarcherStructs::RenderTexture2D& GetSmokeMaskTex();

	void GenerateNoise(ID3D11DeviceContext* aDeviceContext);

	//static void BindRTV(ID3D11DeviceContext* aDeviceContext, int aNumViews);
	static void UnbindRTV(ID3D11DeviceContext* aDeviceContext, int aNumViews);

	//static void BindSRV(ID3D11DeviceContext* aDeviceContext, int aSlot, int aNumViews);
	static void CSUnbindSRV(ID3D11DeviceContext* aDeviceContext, int aStartSlot, int aNumViews);
	static void PSUnbindSRV(ID3D11DeviceContext* aDeviceContext, int aStartSlot, int aNumViews);
	static void VSUnbindSRV(ID3D11DeviceContext* aDeviceContext, int aStartSlot, int aNumViews);

	static void CSUnbindUAV(ID3D11DeviceContext* aDeviceContext, int aStartSlot, int aNumViews);

	static void SetDebugObjectName(ID3D11DeviceChild* aResource, const std::string& aDebugName);
};