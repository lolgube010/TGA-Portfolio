#pragma once
#include <d3d11.h>
#include <xstring>
#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
using Microsoft::WRL::ComPtr;

class FullscreenPass
{
public:
	FullscreenPass() = default;
	void Init(const std::string& aPSPath, ID3D11Device* aDevice);
	void Render(ID3D11DeviceContext* aDeviceContext) const;
private:
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
};
