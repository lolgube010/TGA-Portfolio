#include "Texture.h"
#include <d3d11.h>
#include <vector>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 26819)
#include "stb_image.h"
#pragma warning(pop)
#include <ddsTexLoader/DDSTextureLoader11.h>
#include "CommonUtilities/StringManip.h"
#include "DirectXTexEXR.h"

Texture::Texture() = default;
Texture::~Texture() = default;

bool Texture::Init(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, const int aSlot, const std::string& aTexturePath, const bool aUseSRGB)
{
	// uses DDSTextureLoader11
	if (CommonUtilities::HasExtension(aTexturePath, ".dds"))
	{
		return LoadAndInitDDS(aTexturePath, aDevice, aContext, aSlot);
	}
	// uses DirectXTexEXR
	if (CommonUtilities::HasExtension(aTexturePath, ".exr"))
	{
		throw;
		//return LoadAndInitEXR(aTexturePath, aDevice, aContext, aSlot);
	}
	// uses stbi_load
	return LoadTexture(aTexturePath) && Initialize(aDevice, aContext, aSlot, aUseSRGB);
}

// outside use only!!!
void Texture::BindTexture(ID3D11DeviceContext* aContext)
{
	// this used to be in a Bind() func. moved out of there.
	aContext->PSSetShaderResources(mySlot, 1, myShaderResourceView.GetAddressOf());
}

bool Texture::LoadTexture(const std::string& aTexturePath)
{
	int width, height, channels;
	unsigned char* img = stbi_load(aTexturePath.c_str(), &width, &height, &channels, 0);
	if (img == nullptr)
	{
		FailedError(aTexturePath);
		return false;
	}

	if (channels == 3)
	{
		std::vector<unsigned char> imageData(width * height * 4);

		for (int i = 0; i < width * height; ++i)
		{
			const std::size_t index4 = static_cast<std::size_t>(i) * 4;
			const std::size_t index3 = static_cast<std::size_t>(i) * 3;

			imageData[index4 + 0] = img[index3 + 0];
			imageData[index4 + 1] = img[index3 + 1];
			imageData[index4 + 2] = img[index3 + 2];
			imageData[index4 + 3] = 255;

			// original below removed to fix potental buffer overflow
			/*imageData[4 * i + 0] = img[3 * i + 0];
			imageData[4 * i + 1] = img[3 * i + 1];
			imageData[4 * i + 2] = img[3 * i + 2];
			imageData[4 * i + 3] = 255;*/
		}

		myRgbaPixels = imageData.data();
		myWidth = width;
		myHeight = height;
	}
	else if (channels == 4)
	{
		myRgbaPixels = img; // this is correct
		myWidth = width;
		myHeight = height;
	}
	else
	{
		FailedError(aTexturePath);
		return false;
	}

	return true; // all's good.
}


bool Texture::Initialize(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, const int aSlot, bool aUseSRGB)
{
	ID3D11Texture2D* texture = nullptr;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = myWidth;
	desc.Height = myHeight;
	desc.MipLevels = 0;
	desc.ArraySize = 1;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Format = aUseSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	HRESULT hResult = aDevice->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(hResult))
	{
		MessageBox(nullptr, L"Failed to create Texture2D", L"ERROR", MB_ICONERROR | MB_HELP);
		return false;
	}

	hResult = aDevice->CreateShaderResourceView(texture, nullptr, &myShaderResourceView);
	if (FAILED(hResult))
	{
		MessageBox(nullptr, L"device failed to create shaderResourceView", L"ERROR", MB_ICONERROR | MB_HELP);
		return false;
	}

	aContext->UpdateSubresource(texture, 0, nullptr, (void*)myRgbaPixels, myWidth * 4, myWidth * myHeight * 4);
	aContext->GenerateMips(myShaderResourceView.Get());

	texture->Release();

	mySlot = aSlot;
	// binds our texture
	aContext->PSSetShaderResources(mySlot, 1, myShaderResourceView.GetAddressOf());

	return true;
}

bool Texture::LoadAndInitDDS(const std::string& aTexturePath, ID3D11Device* aDevice, ID3D11DeviceContext* aContext,
	const int aSlot)
{
	// Convert string path to wide string
	const std::wstring wstr = CommonUtilities::StringToWstring(aTexturePath);

	// Load the texture from file
	HRESULT hResult = DirectX::CreateDDSTextureFromFile(
		aDevice,
		aContext,
		wstr.c_str(),
		nullptr,
		&myShaderResourceView);

	if (FAILED(hResult))
	{
		FailedError(aTexturePath);
		return false;
	}

	mySlot = aSlot;
	aContext->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
	return true;
}

//bool Texture::LoadAndInitEXR(const std::string& aTexturePath, ID3D11Device* aDevice, ID3D11DeviceContext* aContext,
//	int aSlot)
//{
//	const std::wstring wstr = CommonUtilities::StringToWstring(aTexturePath);
//
//	auto image = std::make_unique<DirectX::ScratchImage>();
//	auto metaData = std::make_unique<DirectX::TexMetadata>();
//
//	HRESULT result = LoadFromEXRFile(wstr.c_str(), metaData.get(), *image);
//	if (FAILED(result))
//	{
//		FailedError(aTexturePath);
//		return false;
//	}
//
//	result = CreateShaderResourceView(aDevice, image->GetImages(), image->GetImageCount(), *metaData, myShaderResourceView.GetAddressOf());
//	if (FAILED(result))
//	{
//		FailedError(aTexturePath);
//		return false;
//	}
//	mySlot = aSlot;
//	aContext->PSSetShaderResources(aSlot, 1, myShaderResourceView.GetAddressOf());
//
//	return true;
//}

void Texture::FailedError(const std::string& aTexturePath)
{
	if (aTexturePath == "folke.png")
	{
		MessageBox(nullptr, L"please do not remove folke!", L"LOAD BEARING FOLKE.PNG IS MISSING!", MB_ICONERROR | MB_HELP);
	}
	else
	{
		std::string errorMessage = "TEXTURE " + aTexturePath + " IS MISSING FROM GAME! ERROR!";
		std::wstring wideErrorMessage(errorMessage.begin(), errorMessage.end());

		// todo- add "launch anyway" button that defaults to folke tex.
		MessageBox(nullptr, (wideErrorMessage.c_str()), L"ERROR!", MB_ICONERROR | MB_HELP);
	}
}