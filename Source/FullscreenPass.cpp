#include "FullscreenPass.h"
#include <d3d11.h>
#include <fstream>
#include <string>

void FullscreenPass::Init(const std::string& aPSPath, ID3D11Device* aDevice)
{
	{
		HRESULT result;
		std::ifstream vsFile;
		vsFile.open("FullscreenEffect_VS.cso", std::ios::binary);
		std::string data = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = aDevice->CreateVertexShader(data.data(), data.size(), nullptr, &myVertexShader);
		if (FAILED(result)) throw;
		vsFile.close();
	}

	{
		HRESULT result;
		std::ifstream psFile;
		psFile.open(aPSPath + ".cso", std::ios::binary);
		std::string data = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = aDevice->CreatePixelShader(data.data(), data.size(), nullptr, &myPixelShader);
		if (FAILED(result)) throw;
		psFile.close();
	}
}

void FullscreenPass::Render(ID3D11DeviceContext* aDeviceContext) const
{
	aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	aDeviceContext->IASetInputLayout(nullptr);
	aDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	aDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	aDeviceContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
	aDeviceContext->GSSetShader(nullptr, nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader.Get(), nullptr, 0);
	aDeviceContext->Draw(3, 0);
}
