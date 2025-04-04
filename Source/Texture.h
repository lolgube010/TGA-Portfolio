#pragma once
#include <d3d11.h>
#include <string>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;

// TODO- https://freeimage.sourceforge.io/documentation.html

class Texture
{
public:
	Texture();
	~Texture();

	// init runs LoadTexture and then Initialize
	bool Init(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, int aSlot, const std::string& aTexturePath = "dev_256_or_064x.dds", bool aUseSRGB = true);
	void BindTexture(ID3D11DeviceContext* aContext);
private:
	bool LoadTexture(const std::string& aTexturePath);
	bool Initialize(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, int aSlot, bool aUseSRGB = true);

	bool LoadAndInitDDS(const std::string& aTexturePath, ID3D11Device* aDevice, ID3D11DeviceContext* aContext, int aSlot);
	//bool LoadAndInitEXR(const std::string& aTexturePath, ID3D11Device* aDevice, ID3D11DeviceContext* aContext, int aSlot);

	static void FailedError(const std::string& aTexturePath);

	ComPtr<ID3D11ShaderResourceView> myShaderResourceView;
	int mySlot = 0;

	// only used by LoadTexture & Initialize 
	unsigned char* myRgbaPixels = {};
	int myWidth = 0;
	int myHeight = 0;
};