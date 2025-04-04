#include "GraphicsEngine.h"
#include "CommonUtilities/Matrix4x4.h"
#include "Mesh.h"
// Bara för oss själva, denna define och inget som påverkar directX tar bort ett par varningar directx11 har på väldigt många olika saker
#include "Input.h"
#include "Timer.h"
#include <fstream>
#include <d3d11.h>

#include "PortfolioSettings.h"
#include "CommonUtilities/StringManip.h"
#include <iostream>
#include "Buffers.h"
#include "MomoMath.h"

// TODO- ERROR CLASS THAT MANAGES ALL ERROR MESSAGES IN ONE PLACE

bool GraphicsEngine::Initialize(const HWND aWindowHandle)
{
	// save windowhandle ptr for future use
	myWindowHandle = &aWindowHandle;

	// Felrapportering kommer göras genom HRESULT
	HRESULT result;

	if (!CreateDeviceAndSwapChain())
	{
		return false;
	}

	if (!CreateBackBuffer(result))
	{
		return false;
	}
	if (!CreateDepthStencilState(result))
	{
		return false;
	}
	if (!CreateDepthBuffer(result))
	{
		return false;
	}
	// const buffer start
	if (!CreateFrameAndObjectBuffer(result))
	{
		return false;
	}
	// const buffer end
	if (!CreateRasterizerState())
	{
		return false;
	}
	if (!CreateTextureSamplerState(result))
	{
		return false;
	}

	SetupViewport();

	if (!LoadTextures())
	{
		return false;
	}

	if (!myCamera.Init({ 0.f, 0.1f, -2.f }, myBackBufferTextureWidth, myBackBufferTextureHeight, 90, 90, 0.001f, 1000.f))
	{
		return false;
	}

	LoadShaders();

	if (!LoadModels())
	{
		return false;
	}

	myDX11Annotation.Init(myContext);

	PortfolioWorkInit();

	return true;
}

void GraphicsEngine::Update([[maybe_unused]] const float aDeltaTime)
{
	myCamera.Update(globalTimer->GetDeltaTime(), globalInput); // todo- move to gameWorld.Update or something?
	PortfolioWorkUpdate();
}

void GraphicsEngine::Render()
{
	myDX11Annotation.BeginAnnotation(L"GraphicsEngine Render");
	myDx11Timer.BeginFrame();
	//myDefaultTex.BindTexture(myContext.Get());

	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBufferNew.DSV.Get());
	ClearRT();

	UpdateFrameBuffer();

	RenderObject(myWall);
	RenderObject(MyNormalCube);
	//RenderObject(MyNormalCube2);
	RenderObject(myWall2);
	RenderObject(myFloor);
	RenderObject(myNewCube1);
	RenderObject(myNewCube2);
	RenderObject(myNewCube3);
	RenderObject(myNewCube4);
	RenderObject(myNewCube5);
	RenderObject(myTetrahedron);
	RenderObject(myOctahedron);
	myDX11Annotation.EndAnnotation(); // end graphicsEngineRender

	PortfolioWorkRender();
	myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBufferNew.DSV.Get());
	myDx11Timer.EndFrame();
	myDx11Timer.CalculateFrameTime();

	if (!meshBandAid)
		// for some reason, putting voxelizeMeshes in init doesn't run the cs like i'd want it to. something silly's happening but this fixes it for now.
		// don't really have the time to fix it yet :c
	{
		VoxelizeMeshes();
		meshBandAid = true;
	}
}

bool GraphicsEngine::Present()
{
	// present visar ut någt på skrämen (vsync eller inte)
	HRESULT result = PortfolioSettings::vsync
		? mySwapChain->Present(1, 0)
		//: mySwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
		: mySwapChain->Present(0, 0);

	if (FAILED(result))
	{
		MessageBox(nullptr, L"An error occurred while presenting in graphicsEngine.", L"Failed to Present",
			MB_ICONERROR | MB_HELP);
		return false;
	}

	mySwapChain->GetFrameStatistics(&myStats);

	return true;
}

Camera* GraphicsEngine::GetCamera()
{
	return &myCamera;
}

const HWND* GraphicsEngine::GetWindowHandle() const
{
	return myWindowHandle;
}

ComPtr<ID3D11Device> GraphicsEngine::GetDevice()
{
	return myDevice;
}

ComPtr<ID3D11DeviceContext> GraphicsEngine::GetContext()
{
	return myContext;
}

bool GraphicsEngine::CreateDeviceAndSwapChain()
{
	// som vi gör i main.cpp måste vi definera saker, så vi skapar en struct med parametrarna.
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	// SRGB gör att vi får gamma korrigering.
	//swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //this is not supprted with flip mode.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = *myWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	// things imgui does differently
	//ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	//swapChainDesc.BufferCount = 2;
	//swapChainDesc.BufferDesc.Width = 0;
	//swapChainDesc.BufferDesc.Height = 0;
	//swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	//swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	if (!PortfolioSettings::vsync)
	{
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	}
	//swapChainDesc.SampleDesc.Quality = 0;
	//swapChainDesc.Windowed = TRUE;
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // right now we use this
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // we can switch to this if we increase buffer count 1 <

	// vi sätter upp funktionen med en struct istället för massa parametrar då det är mycket lättare att redigera structar än massa parametrar. vi kan skapa en ny struct, om den är större, osv. Det blir lättare att ändra och flytta saker. Man vill kunna stödja det. 

	//UINT creationFlags = 0; 
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// läs dokumentationen om vad allt betyder, lättare än att gå igenom vad alla nullptr betyder.
	// men typ om man har fler grafikkort kan man skicka ni lite fler parametrar än vi gjort här, så man kan välja vilket.

	// result is instantly re-assigned, so why even have the assignment? - momo

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&myDevice,
		nullptr,
		&myContext);
	if (FAILED(result))
	{
		MessageBox(nullptr, L"shit's fucked", L"failed to Create Device And SwapChain!", MB_ICONERROR | MB_HELP);
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateBackBuffer(HRESULT& aHresult)
{
	// Här gör vi inte en comptr av det då vi inte kommer kunna spara undan det så länge, så därav m
	// detta är bra exempel då jag skapar själv en minnesläcka/bugg
	// vi vill ha första texture2D och lägga den i vår backBufferTexture
	aHresult = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(myBackBufferTexture.GetAddressOf()));
	if (FAILED(aHresult))
	{
		MessageBox(nullptr, L"shit's fucked", L"failed to get backbufferTexture!", MB_ICONERROR | MB_HELP);
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC desc; // this was added later to support SRGB
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	aHresult = myDevice->CreateRenderTargetView(myBackBufferTexture.Get(), &desc, myBackBuffer.GetAddressOf());
	if (FAILED(aHresult))
	{
		myBackBufferTexture->Release();
		MessageBox(nullptr, L"shit's fucked", L"failed to create renderTargetView from backBuffer!",
			MB_ICONERROR | MB_HELP);
		return false;
	}

	// här plockar vi ut en beskrivning av texturen för att senare kunna använda den för att t...
	D3D11_TEXTURE2D_DESC textureDesc;
	myBackBufferTexture->GetDesc(&textureDesc);

	myBackBufferTextureWidth = textureDesc.Width;
	myBackBufferTextureHeight = textureDesc.Height;
	return true;
}

bool GraphicsEngine::CreateDepthStencilState(HRESULT& aHresult) const
{
	// Define depth stencil state description
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE; // Disable stencil testing

	// Create depth stencil state
	ID3D11DepthStencilState* depthStencilState;
	aHresult = myDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(aHresult))
	{
		MessageBox(nullptr, L"shit's fucked", L"failed to create depth stencil state!", MB_ICONERROR | MB_HELP);
		return false;
	}

	// Set depth stencil state
	myContext->OMSetDepthStencilState(depthStencilState, 0);

	// Release depth stencil state (optional, depends on usage)
	depthStencilState->Release();
	return true;
}

bool GraphicsEngine::CreateDepthBuffer(HRESULT& aHresult)
{
	// Create depth buffer texture
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = myBackBufferTextureWidth;
	depthBufferDesc.Height = myBackBufferTextureHeight;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	aHresult = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, myDepthBufferNew.Tex.GetAddressOf());
	if (FAILED(aHresult))
	{
		MessageBox(nullptr, L"shit's fucked", L"failed to create depthBufferTexture!", MB_ICONERROR | MB_HELP);
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	aHresult = myDevice->CreateDepthStencilView(myDepthBufferNew.Tex.Get(), &dsvDesc,
		myDepthBufferNew.DSV.GetAddressOf());
	if (FAILED(aHresult))
	{
		MessageBox(nullptr, L"shit's fucked", L"failed to create depthBuffer depthStencilView!",
			MB_ICONERROR | MB_HELP);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = UINT_MAX;

	aHresult = myDevice->CreateShaderResourceView(myDepthBufferNew.Tex.Get(), &srvDesc,
		myDepthBufferNew.SRV.GetAddressOf());
	if (FAILED(aHresult))
	{
		MessageBox(nullptr, L"shit's fucked", L"failed to create depthStencil SRV!", MB_ICONERROR | MB_HELP);
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateFrameAndObjectBuffer(HRESULT& aHresult)
{
	{
		// constant buffers, framebuffer & objectbuffer
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		bufferDesc.ByteWidth = sizeof(FrameBufferData);
		aHresult = myDevice->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
		if (FAILED(aHresult))
		{
			MessageBox(nullptr, L"shit's fucked", L"failed to create frameBuffer!", MB_ICONERROR | MB_HELP);
			return false;
		}

		bufferDesc.ByteWidth = sizeof(ObjectBufferData);
		aHresult = myDevice->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);
		if (FAILED(aHresult))
		{
			MessageBox(nullptr, L"shit's fucked", L"failed to create objectBuffer!", MB_ICONERROR | MB_HELP);
			return false;
		}

		// assuming the layout doesn't change
		myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->CSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
	}
	return true;
}

bool GraphicsEngine::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC wireFrameDesc = {};
	wireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireFrameDesc.CullMode = D3D11_CULL_BACK; // Cull back faces (optional)
	wireFrameDesc.FrontCounterClockwise = FALSE; // Define front face winding
	wireFrameDesc.DepthClipEnable = TRUE; // Enable depth clipping
	HRESULT result = myDevice->CreateRasterizerState(&wireFrameDesc, &myWireFrameRasterizerState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

bool GraphicsEngine::CreateTextureSamplerState(HRESULT& aHresult)
{
	{
		// default tgp sampler
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		aHresult = myDevice->CreateSamplerState(&samplerDesc, &mySamplerState);
		if (FAILED(aHresult))
		{
			MessageBox(nullptr, L"shit's fucked", L"failed to create samplerState!", MB_ICONERROR | MB_HELP);
			return false;
		}
		myContext->PSSetSamplers(0, 1, mySamplerState.GetAddressOf()); // Assuming slot 0
		myContext->CSSetSamplers(0, 1, mySamplerState.GetAddressOf()); // Assuming slot 0
	}
	{
		// PointRepeatSampler
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		aHresult = myDevice->CreateSamplerState(&samplerDesc, &PointRepeatSampler);
		if (FAILED(aHresult))
		{
			MessageBox(nullptr, L"shit's fucked", L"failed to create samplerState!", MB_ICONERROR | MB_HELP);
			return false;
		}
		myContext->PSSetSamplers(1, 1, PointRepeatSampler.GetAddressOf());
		myContext->CSSetSamplers(1, 1, PointRepeatSampler.GetAddressOf());
	}
	{
		// LinearRepeatSampler
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		aHresult = myDevice->CreateSamplerState(&samplerDesc, &LinearRepeatSampler);
		if (FAILED(aHresult))
		{
			MessageBox(nullptr, L"shit's fucked", L"failed to create samplerState!", MB_ICONERROR | MB_HELP);
			return false;
		}
		myContext->PSSetSamplers(2, 1, LinearRepeatSampler.GetAddressOf());
		myContext->CSSetSamplers(2, 1, LinearRepeatSampler.GetAddressOf());
	}

	{
		// PointClampSampler
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		aHresult = myDevice->CreateSamplerState(&samplerDesc, &PointClampSampler);
		if (FAILED(aHresult))
		{
			MessageBox(nullptr, L"shit's fucked", L"failed to create samplerState!", MB_ICONERROR | MB_HELP);
			return false;
		}
		myContext->PSSetSamplers(3, 1, PointClampSampler.GetAddressOf());
		myContext->CSSetSamplers(3, 1, PointClampSampler.GetAddressOf());
	}
	{
		// LinearClampSampler
		D3D11_SAMPLER_DESC samplerDesc{};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		aHresult = myDevice->CreateSamplerState(&samplerDesc, &LinearClampSampler);
		if (FAILED(aHresult))
		{
			MessageBox(nullptr, L"shit's fucked", L"failed to create samplerState!", MB_ICONERROR | MB_HELP);
			return false;
		}
		myContext->PSSetSamplers(4, 1, LinearClampSampler.GetAddressOf());
		myContext->CSSetSamplers(4, 1, LinearClampSampler.GetAddressOf());
	}
	return true;
}

void GraphicsEngine::SetupViewport() const
{
	// ReSharper disable once CppInitializedValueIsAlwaysRewritten
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(myBackBufferTextureWidth);
	viewport.Height = static_cast<float>(myBackBufferTextureHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	myContext->RSSetViewports(1, &viewport);
}

bool GraphicsEngine::LoadTextures()
{
	//if (!myDefaultTex.Init(myDevice.Get(), myContext.Get(), 0))
	//{
	//	return false;
	//}
	return true;
}

void GraphicsEngine::LoadShaders()
{
	// ps
	CreatePixelShader(myDevice.Get(), "MeshPS", myMeshPS);

	CreateVertexShaderAndMaybeInputLayout(myDevice.Get(), "MeshVS", myMeshVS, &myDefaultInputLayout);
}

bool GraphicsEngine::LoadModels()
{
	if (!myWall.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}
	myWall.SetScale({ 5, 5, 1 });
	myWall.SetPosition({ 0, 3, 5 });
	if (!MyNormalCube.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
		//if (!MyNormalAssCube.Init(myDevice.Get(), "MeshPS", "Testing_VS", true))
	{
		return false;
	}
	MyNormalCube.SetPosition(-7, 0, -7);

	//if (!MyNormalCube2.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	//{
	//	return false;
	//}
	//MyNormalCube2.SetPosition(7, 0, -7);


	if (!myWall2.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}
	myWall2.SetScale({ 5, 5, 1 });
	myWall2.SetPosition({ 0, 2, -5 });

	if (!myFloor.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout))
	{
		return false;
	}

	if (!myNewCube1.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}

	constexpr float dist = 2;
	myNewCube1.SetScale({ 5,5,1 });
	myNewCube1.SetPosition({ -10,1,10 + dist });

	if (!myNewCube2.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}
	myNewCube2.SetScale({ 5,5,1 });
	myNewCube2.SetPosition({ -10,1,10 - dist });

	if (!myNewCube3.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}
	myNewCube3.SetScale({ 1,5,5 });
	myNewCube3.SetPosition({ -(10 + dist),1,10 });

	if (!myNewCube4.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}
	myNewCube4.SetScale({ 1,5,5 });
	myNewCube4.SetPosition({ -(10 - dist),1,10 });

	if (!myNewCube5.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}
	myNewCube5.SetScale({ 7,1,7 });
	myNewCube5.SetPosition({ 10,2,-10 });

	if (!myOctahedron.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}
	myOctahedron.SetScale({ 5,5,5 });
	myOctahedron.SetPosition(-10, 5, -10);

	if (!myTetrahedron.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout, true))
	{
		return false;
	}
	myTetrahedron.SetScale({ 5,5,5 });
	myTetrahedron.SetPosition(10, 2.5f, 10);

	return true;
}

void GraphicsEngine::UpdateFrameBuffer() const
{
	// Prepare FrameBufferData
	// This sets up the camera on the GPU
	FrameBufferData frameBufferData = {};
	frameBufferData.worldToClipMatrix = myCamera.GetWorldToClipMatrix();
	frameBufferData.time = globalTimer->GetTotalSecondsFloat();
	frameBufferData.camPos = myCamera.GetTransform().GetPosition();
	frameBufferData.cameraMatrix = myCamera.GetTransform();
	frameBufferData.projectionMatrix = myCamera.GetProjectionMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));
	myContext->Unmap(myFrameBuffer.Get(), 0);

	// they do this in tgp tutorial, we do it in createObjectBuffer since we assume the layout doesn't change.
	myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	myContext->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
}

void GraphicsEngine::UpdateObjectBuffer(const Mesh& aObject) const
{
	// Prepare ObjectBufferData
	// This sets up the transform of the object on the GPU
	ObjectBufferData objectBufferData;
	objectBufferData.modelToWorldMatrix = aObject.GetTransform();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myContext->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &objectBufferData, sizeof(ObjectBufferData));
	myContext->Unmap(myObjectBuffer.Get(), 0);

	// they do this in tgp tutorial, we do it in createObjectBuffer since we assume the layout doesn't change.
	myContext->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
}

void GraphicsEngine::RenderObject(Mesh& aObject) const
{
	UpdateObjectBuffer(aObject);

	aObject.Render(myContext.Get());
}

void GraphicsEngine::RenderObjectInstanced(Mesh& aObject, const UINT anInstanceCount) const
{
	UpdateObjectBuffer(aObject);

	aObject.RenderInstanced(myContext.Get(), anInstanceCount);
}

void GraphicsEngine::ClearRT()
{
	// tar vår backbuffer och fyller den
	myContext->ClearRenderTargetView(myBackBuffer.Get(), myBGColor);
	myContext->ClearDepthStencilView(
		myDepthBufferNew.DSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);
	//myContext->RSSetState(nullptr);
}

void GraphicsEngine::PortfolioWorkInit()
{
	D3D11DebugBreakShit();
	myDx11Timer.Init(myDevice.Get(), myContext.Get());
	myVoxelGrid.Init(myDevice, myDefaultInputLayout);

	// fill the data we wanna voxelize!

	if (!myRayCastHitCube.Init(myDevice.Get(), myMeshPS, myMeshVS, myDefaultInputLayout))
	{
		throw;
	}
	myRayCastHitCube.SetScale(.3f);

	InitPortfolioCBuffer();

	InitVoxelBuffer();
	CreateComputeShader(myDevice.Get(), "VoxelGrid_CS.cso", myVoxelComputeShader);

	CreateComputeShader(myDevice.Get(), "MeshVoxelizer_CS.cso", myMeshVoxelizerComputeShader);
	InitMeshVoxelizerBuffer();

	CreateComputeShader(myDevice.Get(), "VoxelGridClear_CS.cso", myVoxelClearComputeShader);
	InitSmokeBuffer(); // binds to slots 1 in cs.
	InitRayMarcher();
}

void GraphicsEngine::InitPortfolioCBuffer()
{
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDesc.ByteWidth = sizeof(PortfolioBufferData);
	HRESULT res = myDevice->CreateBuffer(&bufferDesc, nullptr, myPortfolioConstBuffer.GetAddressOf());
	if (FAILED(res))
	{
		MessageBox(nullptr, L"shit's fucked", L"failed to create portfolioBuffer!", MB_ICONERROR | MB_HELP);
		throw;
	}

	// assuming the layout doesn't change
	//myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	//myContext->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	myContext->CSSetConstantBuffers(2, 1, myPortfolioConstBuffer.GetAddressOf());
	myContext->PSSetConstantBuffers(2, 1, myPortfolioConstBuffer.GetAddressOf());
	myContext->VSSetConstantBuffers(2, 1, myPortfolioConstBuffer.GetAddressOf());

	FillPortfolioCBuffer();
}

void GraphicsEngine::FillPortfolioCBuffer()
{
	PortfolioBufferData data;
	data.boundingBoxExtents = myVoxelGrid.myBBExtents;
	data.boundingBoxGridRes = myVoxelGrid.myVoxelGridRes;
	data.voxelSize = PortfolioSettings::voxelSize;
	data.voxelCount = myVoxelGrid.GetElementCount();
	data.smokePos = PortfolioSettings::smoke.pos;
	data.smokeRadius = PortfolioSettings::smoke.radius;
	data.smokeTimer = PortfolioSettings::smoke.timer;
	data.meshesToBeVoxelizedTriCount = static_cast<unsigned int>(myMeshesToBeVoxelized.size());
	data.drawSmokeVoxels = PortfolioSettings::drawSmokeVoxels;
	data.drawEdgeVoxels = PortfolioSettings::drawEdgeVoxels;
	data.drawStaticVoxels = PortfolioSettings::drawStaticVoxels;
	data.drawSmokeSeed = PortfolioSettings::drawSmokeSeed;
	data.backBufferSize = { myBackBufferTextureWidth, myBackBufferTextureHeight };
	data.smokeColor = PortfolioSettings::smoke.color;
	data.absorptionCoefficient = PortfolioSettings::absorptionCoefficient;
	data.scatteringCoefficient = PortfolioSettings::scatteringCoefficient;
	data.stepSize = PortfolioSettings::stepSize;
	data.lightStepSize = PortfolioSettings::lightStepSize;
	data.alphaThreshold = PortfolioSettings::alphaThreshold;
	data.stepCount = PortfolioSettings::stepCount;
	data.volumeDensity = PortfolioSettings::volumeDensity;
	data.densityFallOff = PortfolioSettings::densityFalloff;
	data.smokeSize = PortfolioSettings::smokeSize;
	data.animDirection = PortfolioSettings::animDirection;
	data.octaves = PortfolioSettings::octaves;
	data.warp = PortfolioSettings::warp;
	data.axisCellCount = PortfolioSettings::axisCellCount;
	data.add = PortfolioSettings::add;
	data.amplitude = PortfolioSettings::amplitude;
	data.invertNoise = PortfolioSettings::invertNoise;
	data.cellSize = PortfolioSettings::cellSize;
	data.noiseSeed = PortfolioSettings::seed;
	data.sharpness = PortfolioSettings::sharpness;
	data.sunPos = PortfolioSettings::sun.pos;
	data.lightStepCount = PortfolioSettings::lightStepCount;
	data.debugView = PortfolioSettings::debugView;
	data.scatteringAnisotropy = PortfolioSettings::scatteringAnisotropy;
	data.phaseFunc = static_cast<int>(PortfolioSettings::phaseFunc);
	data.shadowDensity = PortfolioSettings::shadowDensity;
	data.extinctionColor = PortfolioSettings::extinctionColor;
	data.sunColor = PortfolioSettings::sun.color;
	data.smokeRes = static_cast<int>(PortfolioSettings::resScale);
	data.activateTonemap = PortfolioSettings::activateTonemap;
	data.rayMarchGrowthSteepness = PortfolioSettings::rayMarchGrowthSteepness;
	data.voxelGrowthSteepness = PortfolioSettings::voxelGrowthSteepness;
	data.InterleavedGradientNoiseStrength = PortfolioSettings::InterleavedGradientNoiseStrength;
	data.maskInfluence = PortfolioSettings::maskInfluence;
	data.biTagentNoiseScale = PortfolioSettings::biTagentNoiseScale;
	data.biTagentNoiseStrength = PortfolioSettings::biTagentNoiseStrength;

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myContext->Map(myPortfolioConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &data, sizeof(PortfolioBufferData));
	myContext->Unmap(myPortfolioConstBuffer.Get(), 0);

	myContext->CSSetConstantBuffers(2, 1, myPortfolioConstBuffer.GetAddressOf());
	myContext->PSSetConstantBuffers(2, 1, myPortfolioConstBuffer.GetAddressOf());
	myContext->VSSetConstantBuffers(2, 1, myPortfolioConstBuffer.GetAddressOf());
}

void GraphicsEngine::InitSmokeBuffer()
{
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(SmokeBufferData);
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(SmokeBufferData);

	SmokeBufferData data;
	data.totalFuel = 1000;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &data;
	HRESULT res = myDevice->CreateBuffer(&bufferDesc, &initData, mySmokeBuffer.GetAddressOf());
	if (FAILED(res)) throw;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = 1;

	res = myDevice->CreateShaderResourceView(mySmokeBuffer.Get(), &srvDesc, mySmokeBufferSRV.GetAddressOf());
	if (FAILED(res)) throw;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = 1;
	res = myDevice->CreateUnorderedAccessView(mySmokeBuffer.Get(), &uavDesc, mySmokeBufferUAV.GetAddressOf());
	if (FAILED(res)) throw;

	// bind them since we only need to do this once tbh
	//myContext->CSSetShaderResources(1, 1, mySmokeBufferSRV.GetAddressOf());
	myContext->CSSetUnorderedAccessViews(1, 1, mySmokeBufferUAV.GetAddressOf(), nullptr);

	//DebugSmokeBuffer();
	//FillSmokeBuffer();
}

void GraphicsEngine::FillSmokeBuffer()
{
	SmokeBufferData data;
	data.totalFuel = 1000;

	// should be the same as the code below
	myContext->UpdateSubresource(mySmokeBuffer.Get(), 0, nullptr, &data, 0, 0);
}

void GraphicsEngine::InitVoxelBuffer()
{
	auto elementCount = myVoxelGrid.GetElementCount();

	// compute buffer desc
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(momo_portfolio::Voxel) * elementCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(momo_portfolio::Voxel);

	// set up data to fill the vec with
	D3D11_SUBRESOURCE_DATA initDataD3D11 = {};
	initDataD3D11.pSysMem = myVoxelGrid.voxelData.data();

	HRESULT result = myDevice->CreateBuffer(&bufferDesc, &initDataD3D11, myVoxelComputeBuffer.GetAddressOf());
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create compute buffer", L"Error", MB_ICONERROR);
		return;
	}

	// Create an Unordered Access View (UAV) for writing to the buffer
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN; // Structured buffers must use UNKNOWN format
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = elementCount;
	uavDesc.Buffer.FirstElement = 0;

	result = myDevice->CreateUnorderedAccessView(myVoxelComputeBuffer.Get(), &uavDesc, myVoxelBufferUAV.GetAddressOf());
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create UAV", L"Error", MB_ICONERROR);
		return;
	}

	// Create a Shader Resource View (SRV)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;

	result = myDevice->CreateShaderResourceView(myVoxelComputeBuffer.Get(), &srvDesc, myVoxelBufferSRV.GetAddressOf());
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create SRV", L"Error", MB_ICONERROR);
	}
}

void GraphicsEngine::PortfolioWorkUpdate()
{
	if (globalInput->GetIsKeyDown(KeyCode::MiddleMouseButton) || globalInput->GetIsKeyDown(KeyCode::G))
	{
		DirectX::XMFLOAT3 aOutIntersectionPoint;
		if (RayCastFromCamera(aOutIntersectionPoint))
		{
			myRayCastHitCube.SetPositionXMFLOAT3(aOutIntersectionPoint);
			SpawnSmokeGrenade(aOutIntersectionPoint);
		}
	}
	PortfolioSettings::smoke.timer += globalTimer->GetDeltaTime();
}

bool GraphicsEngine::RayCastFromCamera(DirectX::XMFLOAT3& aOutIntersectionPoint)
{
	auto originCamPos = myCamera.GetTransform().GetPositionVec4();
	DirectX::FXMVECTOR origin = { originCamPos.x, originCamPos.y, originCamPos.z, originCamPos.w };
	auto camDir = myCamera.GetCamForward();
	DirectX::FXMVECTOR direction = { camDir.x, camDir.y, camDir.z };
	float outDist = 0;
	if (myFloor.myBoundingBox.Intersects(origin, direction, outDist))
	{
		aOutIntersectionPoint = MomoMath::GetIntersectionPoint(origin, direction, outDist);
		return true;
	}
	return false;
}

void GraphicsEngine::SpawnSmokeGrenade(const DirectX::XMFLOAT3 aOrigin)
{
	PortfolioSettings::smoke.pos = aOrigin;
	PortfolioSettings::smoke.timer = 0; // reset timer!
	RunVoxelClearComputeShader();
}

void GraphicsEngine::RunVoxelComputeShader()
{
	// dispatch compute shader
	{
		myContext->CSSetShader(myVoxelComputeShader.Get(), nullptr, 0);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		myContext->VSSetShaderResources(0, 1, nullSRV); // Unbind SRV from vertex shader
		myContext->PSSetShaderResources(0, 1, nullSRV); // Unbind SRV from vertex shader

		myContext->CSSetUnorderedAccessViews(0, 1, myVoxelBufferUAV.GetAddressOf(), nullptr);

		myContext->Dispatch(static_cast<int>(std::ceilf(myVoxelGrid.GetElementCount() / 128.f)), 1, 1);
		//myContext->Dispatch(myVoxelGrid.myVoxelGridRes.x, myVoxelGrid.myVoxelGridRes.y, myVoxelGrid.myVoxelGridRes.z);

		ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
		myContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}

	myContext->VSSetShaderResources(0, 1, myVoxelBufferSRV.GetAddressOf());
	myContext->PSSetShaderResources(0, 1, myVoxelBufferSRV.GetAddressOf());
}

void GraphicsEngine::RunVoxelClearComputeShader()
{
	// dispatch compute shader
	{
		myContext->CSSetShader(myVoxelClearComputeShader.Get(), nullptr, 0);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		myContext->VSSetShaderResources(0, 1, nullSRV); // Unbind SRV from vertex shader
		myContext->CSSetShaderResources(0, 1, nullSRV);
		myContext->PSSetShaderResources(0, 1, nullSRV); // Pixel shader
		ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
		myContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

		myContext->CSSetUnorderedAccessViews(0, 1, myVoxelBufferUAV.GetAddressOf(), nullptr);

		myContext->Dispatch(static_cast<UINT>(myVoxelGrid.myVoxelGridRes.x),
			static_cast<UINT>(myVoxelGrid.myVoxelGridRes.y),
			static_cast<UINT>(myVoxelGrid.myVoxelGridRes.z));

		myContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	}

	myContext->VSSetShaderResources(0, 1, myVoxelBufferSRV.GetAddressOf());
	myContext->PSSetShaderResources(0, 1, myVoxelBufferSRV.GetAddressOf());
}

void GraphicsEngine::InitMeshVoxelizerBuffer()
{
	UpdateMeshesToBeVoxelized();

	// compute buffer desc
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(Mesh::MeshTri) * static_cast<UINT>(myMeshesToBeVoxelized.size()); // Size in bytes
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(Mesh::MeshTri);

	// set up data to fill the vec with
	D3D11_SUBRESOURCE_DATA initDataD3D11 = {};
	initDataD3D11.pSysMem = myMeshesToBeVoxelized.data();

	HRESULT result = myDevice->CreateBuffer(&bufferDesc, &initDataD3D11, myMeshVoxelizerBuffer.GetAddressOf());
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create compute buffer", L"Error", MB_ICONERROR);
		return;
	}

	// Create a Shader Resource View (SRV)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(myMeshesToBeVoxelized.size());

	result = myDevice->CreateShaderResourceView(myMeshVoxelizerBuffer.Get(), &srvDesc,
		myMeshVoxelizerSRV.GetAddressOf());
	if (FAILED(result))
	{
		MessageBox(nullptr, L"Failed to create SRV", L"Error", MB_ICONERROR);
	}
}

void GraphicsEngine::ToggleWireFrame() const
{
	if (!PortfolioSettings::wireframe)
	{
		myContext->RSSetState(myWireFrameRasterizerState.Get());
		PortfolioSettings::wireframe = true;
	}
	else
	{
		myContext->RSSetState(nullptr);
		PortfolioSettings::wireframe = false;
	}
}

void GraphicsEngine::PortfolioWorkRender()
{
	myDX11Annotation.BeginAnnotation(L"PortfolioWorkRender");
	FillPortfolioCBuffer();

	if (PortfolioSettings::drawRayCube)
	{
		myDX11Annotation.BeginAnnotation(L"RayCast Cube");
		RenderObject(myRayCastHitCube);
		myDX11Annotation.EndAnnotation();
	}

	RunVoxelComputeShader();

	if (PortfolioSettings::drawVoxels)
	{
		RenderObjectInstanced(*myVoxelGrid.GetVoxelMesh(), myVoxelGrid.GetElementCount());
	}
	// run smoke cs here
	myDX11Annotation.BeginAnnotation(L"RayMarcher");
	myRayMarcher.Render(myContext.Get(), myDepthBufferNew, myVoxelBufferSRV, myBackBuffer, myBackBufferTexture,
		myVoxelBufferUAV, myDevice.Get());
	myDX11Annotation.EndAnnotation();
	myDX11Annotation.EndAnnotation();
}

void GraphicsEngine::D3D11DebugBreakShit()
{
	// d3d11 debug, replace with class from grupprojekt l8r:
	ID3D11Debug* debug = nullptr;
	if (SUCCEEDED(myDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug))))
	{
		ID3D11InfoQueue* infoQueue = nullptr;
		if (SUCCEEDED(debug->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&infoQueue))))
		{
			// Enable break on errors
			infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);

			// Enable break on warnings
			infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, TRUE);

			// Enable break on corruption
			infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);

			infoQueue->Release();
		}
		debug->Release();
	}
}

void GraphicsEngine::LogBoundResources()
{
	// Query bound SRVs (slots 0-7 as an example, adjust based on your needs)
	ComPtr<ID3D11ShaderResourceView> srvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
	myContext->CSGetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, srvs->GetAddressOf());

	std::cout << "Bound SRVs:\n";
	for (int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; ++i)
	{
		if (srvs[i])
		{
			std::cout << "Slot " << i << ": SRV bound\n";
			// Optionally, get the resource description for more details
			ComPtr<ID3D11Resource> resource;
			srvs[i]->GetResource(&resource);
			if (resource)
			{
				D3D11_RESOURCE_DIMENSION dim;
				resource->GetType(&dim);
				std::cout << "  Resource Type: " << dim << "\n";
			}
		}
		else
		{
			std::cout << "Slot " << i << ": No SRV bound\n";
		}
	}

	// Query bound UAVs (slots 0-7 as an example)
	ComPtr<ID3D11UnorderedAccessView> uavs[D3D11_PS_CS_UAV_REGISTER_COUNT];
	myContext->CSGetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, uavs->GetAddressOf());

	std::cout << "Bound UAVs:\n";
	for (int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; ++i)
	{
		if (uavs[i])
		{
			std::cout << "Slot " << i << ": UAV bound\n";
			ComPtr<ID3D11Resource> resource;
			uavs[i]->GetResource(&resource);
			if (resource)
			{
				D3D11_RESOURCE_DIMENSION dim;
				resource->GetType(&dim);
				std::cout << "  Resource Type: " << dim << "\n";
			}
		}
		else
		{
			std::cout << "Slot " << i << ": No UAV bound\n";
		}
	}
}

void GraphicsEngine::DebugSmokeBuffer()
{
	// Create a staging buffer
	D3D11_BUFFER_DESC stagingDesc = {};
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.ByteWidth = sizeof(SmokeBufferData);
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	ComPtr<ID3D11Buffer> stagingBuffer;
	myDevice->CreateBuffer(&stagingDesc, nullptr, &stagingBuffer);

	// Copy mySmokeBuffer to staging buffer
	myContext->CopyResource(stagingBuffer.Get(), mySmokeBuffer.Get());

	// Map and read
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (SUCCEEDED(myContext->Map(stagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
	{
		auto data = static_cast<SmokeBufferData*>(mapped.pData);
		std::cout << "totalFuel from GPU: " << data->totalFuel << "\n";
		myContext->Unmap(stagingBuffer.Get(), 0);
	}
	else
	{
		std::cerr << "Failed to map staging buffer!\n";
	}
}


void GraphicsEngine::RunMeshVoxelizerComputeShader()
{
	// dispatch compute shader?
	{
		myContext->CSSetShader(myMeshVoxelizerComputeShader.Get(), nullptr, 0);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		myContext->VSSetShaderResources(0, 1, nullSRV); // Unbind SRV from vertex shader
		myContext->CSSetShaderResources(0, 1, nullSRV);
		myContext->PSSetShaderResources(0, 1, nullSRV); // Pixel shader
		ID3D11UnorderedAccessView* nullUAV[] = { nullptr };
		myContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

		myContext->CSSetUnorderedAccessViews(0, 1, myVoxelBufferUAV.GetAddressOf(), nullptr);

		myContext->CSSetShaderResources(0, 1, myMeshVoxelizerSRV.GetAddressOf());

		myContext->Dispatch(static_cast<int>(std::ceilf(myVoxelGrid.GetElementCount() / 128.f)), 1, 1);

		// unbind?
		myContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		myContext->CSSetShaderResources(0, 1, nullSRV);
	}
}

void GraphicsEngine::UpdateMeshesToBeVoxelized()
{
	myMeshesToBeVoxelized.clear();
	auto wallMeshWorldSpace = myWall.GetMeshTriangles(true);
	auto wall2MeshWorldSpace = myWall2.GetMeshTriangles(true);
	auto normalAssCube = MyNormalCube.GetMeshTriangles(true);

	auto octahedron = myOctahedron.GetMeshTriangles(true);
	auto tetrahedron = myTetrahedron.GetMeshTriangles(true);
	auto newCube1 = myNewCube1.GetMeshTriangles(true);
	auto newCube2 = myNewCube2.GetMeshTriangles(true);
	auto newCube3 = myNewCube3.GetMeshTriangles(true);
	auto newCube4 = myNewCube4.GetMeshTriangles(true);
	auto newCube5 = myNewCube5.GetMeshTriangles(true);

	// fill the data we wanna voxelize!
	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		wallMeshWorldSpace.begin(), // Start of source vector
		wallMeshWorldSpace.end() // End of source vector
	);
	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		wall2MeshWorldSpace.begin(), // Start of source vector
		wall2MeshWorldSpace.end() // End of source vector
	);
	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		normalAssCube.begin(), // Start of source vector
		normalAssCube.end() // End of source vector
	);

	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		octahedron.begin(), // Start of source vector
		octahedron.end() // End of source vector
	);

	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		tetrahedron.begin(), // Start of source vector
		tetrahedron.end() // End of source vector
	);

	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		newCube1.begin(), // Start of source vector
		newCube1.end() // End of source vector
	);

	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		newCube2.begin(), // Start of source vector
		newCube2.end() // End of source vector
	);

	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		newCube3.begin(), // Start of source vector
		newCube3.end() // End of source vector
	);

	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		newCube4.begin(), // Start of source vector
		newCube4.end() // End of source vector
	);

	myMeshesToBeVoxelized.insert(
		myMeshesToBeVoxelized.end(), // Insert at the end
		newCube5.begin(), // Start of source vector
		newCube5.end() // End of source vector
	);
}

void GraphicsEngine::UpdateMeshesToBeVoxelizedBuffer() const
{
	D3D11_SUBRESOURCE_DATA updatedData;
	updatedData.pSysMem = myMeshesToBeVoxelized.data(); // Same data as before

	myContext->UpdateSubresource(
		myMeshVoxelizerBuffer.Get(), // The buffer to update
		0, // Subresource index (0 for a buffer)
		nullptr, // Optional D3D11_BOX for partial update (nullptr for full update)
		updatedData.pSysMem, // Pointer to the data
		0, // Row pitch (not used for buffers)
		0 // Depth pitch (not used for buffers)
	);
}

void GraphicsEngine::InitRayMarcher()
{
	myRayMarcher.Init(myDevice.Get(), myContext.Get(), myBackBufferTextureWidth, myBackBufferTextureHeight);
}

void GraphicsEngine::VoxelizeMeshes()
{
	UpdateMeshesToBeVoxelized();
	UpdateMeshesToBeVoxelizedBuffer();
	RunMeshVoxelizerComputeShader();
	//std::vector<Mesh*> meshesToVoxelize;
	//meshesToVoxelize.push_back(&myWall);
	//Voxelize::VoxelizeMeshes(meshesToVoxelize);
}

void GraphicsEngine::CreateComputeShader(ID3D11Device* aDevice, const std::string& aShaderName,
	ComPtr<ID3D11ComputeShader>& aCsToSet)
{
	std::ifstream csFile;
	csFile.open(aShaderName.c_str(), std::ios::binary);
	std::string csData = { std::istreambuf_iterator(csFile), std::istreambuf_iterator<char>() };
	if (csData.empty())
	{
		throw;
	}

	HRESULT result = aDevice->CreateComputeShader(csData.data(), csData.size(), nullptr, aCsToSet.GetAddressOf());
	if (FAILED(result))
	{
		throw;
	}

	csFile.close();
}

void GraphicsEngine::CreatePixelShader(ID3D11Device* aDevice, const std::string& aShaderName,
	ComPtr<ID3D11PixelShader>& aPsToSet)
{
	std::ifstream psFile;
	psFile.open(aShaderName + ".cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator(psFile), std::istreambuf_iterator<char>() };
	HRESULT result = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, aPsToSet.GetAddressOf());
	if (FAILED(result))
	{
		throw;
	}
	psFile.close();
}

void GraphicsEngine::CreateVertexShaderAndMaybeInputLayout(ID3D11Device* aDevice, const std::string& aShaderName,
	ComPtr<ID3D11VertexShader>& aVsToSet,
	ComPtr<ID3D11InputLayout>* aInputLayoutToCreate)
{
	std::string vsData;

	std::ifstream vsFile;
	vsFile.open(aShaderName + ".cso", std::ios::binary);
	vsData = { std::istreambuf_iterator(vsFile), std::istreambuf_iterator<char>() };
	HRESULT result = aDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, aVsToSet.GetAddressOf());
	if (FAILED(result))
	{
		throw;
	}
	vsFile.close();

	if (aInputLayoutToCreate)
	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{
				"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{
				"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{
				"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},
			{"SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		result = aDevice->CreateInputLayout(layout, std::size(layout), vsData.data(), vsData.size(),
			aInputLayoutToCreate->GetAddressOf());
		if (FAILED(result))
		{
			throw;
		}
	}
}
