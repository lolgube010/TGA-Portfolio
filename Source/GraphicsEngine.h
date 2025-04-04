#pragma once
// behöver inte inkludera dx11 i header filer, och undvik det dessutom
#include <wrl/client.h>
#include "Mesh.h"
#include "Camera.h"
#include "Primitive.h"
#include "Texture.h"
//#include "Triangle.h"
#include "DX11Annotation.h"
#include "DX11Timer.h"
#include "RayMarcher.h"
#include "VoxelGrid.h"

class GraphicsEngine
{
public:
	GraphicsEngine() = default;
	~GraphicsEngine() = default;

	bool Initialize(HWND aWindowHandle);
	void Update(float aDeltaTime);
	void Render();

	bool Present();

	//void OnResize(); // todo

	Camera* GetCamera();
	const HWND* GetWindowHandle() const;

	ComPtr<ID3D11Device> GetDevice();
	ComPtr<ID3D11DeviceContext> GetContext();
	Texture myDefaultTex;

	Primitive<SimplePlane<30.f, 30.f>> myFloor;

	// camera
	Camera myCamera;

	// pixel shaders
	ComPtr<ID3D11PixelShader> myMeshPS;

	// vertex shaders
	ComPtr<ID3D11VertexShader> myMeshVS;

	// default input layouts
	ComPtr<ID3D11InputLayout> myDefaultInputLayout;

	float myBGColor[4] = {0.184f, 0.031f, 0.22f, 1.0f}; // RGBA
	// valfri färg i RGBA (for bg)

	bool CreateDeviceAndSwapChain();
	bool CreateBackBuffer(HRESULT& aHresult);
	bool CreateDepthStencilState(HRESULT& aHresult) const;
	bool CreateDepthBuffer(HRESULT& aHresult);
	bool CreateFrameAndObjectBuffer(HRESULT& aHresult);
	bool CreateRasterizerState();
	bool CreateTextureSamplerState(HRESULT& aHresult);

	void SetupViewport() const;

	bool LoadTextures();
	void LoadShaders(); // also loads input layout cuz fuck it i guess
	bool LoadModels();

	void UpdateFrameBuffer() const;
	void UpdateObjectBuffer(const Mesh& aObject) const;
	void RenderObject(Mesh& aObject) const;
	void RenderObjectInstanced(Mesh& aObject, UINT anInstanceCount) const;
	void ClearRT();

	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;

	ComPtr<ID3D11RenderTargetView> myBackBuffer;
	ComPtr<ID3D11Texture2D> myBackBufferTexture;
	RayMarcherStructs::DepthBuffer myDepthBufferNew;

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;

	ComPtr<ID3D11SamplerState> mySamplerState;

	ComPtr<ID3D11SamplerState> PointRepeatSampler;
	ComPtr<ID3D11SamplerState> LinearRepeatSampler;
	ComPtr<ID3D11SamplerState> PointClampSampler;
	ComPtr<ID3D11SamplerState> LinearClampSampler;

	ComPtr<ID3D11RasterizerState> myWireFrameRasterizerState;

	const HWND* myWindowHandle = nullptr;

	UINT myBackBufferTextureWidth = 0;
	UINT myBackBufferTextureHeight = 0;

	// PORTFOLIO OBJECTS START //
	momo_portfolio::VoxelGrid myVoxelGrid;

	RayMarcher myRayMarcher;

	void PortfolioWorkInit();
	void PortfolioWorkUpdate();
	void PortfolioWorkRender();

	void InitPortfolioCBuffer();
	void FillPortfolioCBuffer();

	void InitSmokeBuffer();
	void FillSmokeBuffer();

	void InitVoxelBuffer();

	void RunVoxelComputeShader();

	void RunVoxelClearComputeShader();

	void InitMeshVoxelizerBuffer();
	void RunMeshVoxelizerComputeShader();

	void UpdateMeshesToBeVoxelized();
	void UpdateMeshesToBeVoxelizedBuffer() const;

	void InitRayMarcher();

	ComPtr<ID3D11Buffer> myPortfolioConstBuffer = nullptr;

	ComPtr<ID3D11Buffer> myVoxelComputeBuffer = nullptr;
	// this buffer is honestly just passed from the cs to the vs, also written to by our meshVoxelizer
	ComPtr<ID3D11UnorderedAccessView> myVoxelBufferUAV = nullptr;
	ComPtr<ID3D11ShaderResourceView> myVoxelBufferSRV = nullptr;

	ComPtr<ID3D11Buffer> mySmokeBuffer = nullptr; // buffer for smoke settings
	ComPtr<ID3D11UnorderedAccessView> mySmokeBufferUAV = nullptr;
	ComPtr<ID3D11ShaderResourceView> mySmokeBufferSRV = nullptr;

	ComPtr<ID3D11ComputeShader> myVoxelComputeShader = nullptr;
	ComPtr<ID3D11ComputeShader> myVoxelClearComputeShader = nullptr;

	Primitive<Cube> myRayCastHitCube;

	bool RayCastFromCamera(DirectX::XMFLOAT3& aOutIntersectionPoint);
	void SpawnSmokeGrenade(DirectX::XMFLOAT3 aOrigin);

	void D3D11DebugBreakShit();
	void LogBoundResources();
	void DebugSmokeBuffer();

	std::vector<Mesh::MeshTri> myMeshesToBeVoxelized;
	ComPtr<ID3D11Buffer> myMeshVoxelizerBuffer = nullptr;
	ComPtr<ID3D11ShaderResourceView> myMeshVoxelizerSRV = nullptr;
	ComPtr<ID3D11ComputeShader> myMeshVoxelizerComputeShader = nullptr;

	void ToggleWireFrame() const;
	void VoxelizeMeshes();
	DXGI_FRAME_STATISTICS myStats;
	DX11Timer myDx11Timer;
	DX11Annotation myDX11Annotation;

	Primitive<Cube> myWall;
	Primitive<Cube> myWall2;
	Primitive<Cube> MyNormalCube;
	//Primitive<Cube> MyNormalCube2;
	Primitive<Cube> myNewCube1;
	Primitive<Cube> myNewCube2;
	Primitive<Cube> myNewCube3;
	Primitive<Cube> myNewCube4;
	Primitive<Cube> myNewCube5;
	Primitive<Octahedron> myOctahedron;
	Primitive<Tetrahedron> myTetrahedron;

	static void CreateComputeShader(ID3D11Device* aDevice, const std::string& aShaderName,
	                                ComPtr<ID3D11ComputeShader>& aCsToSet);
	static void CreatePixelShader(ID3D11Device* aDevice, const std::string& aShaderName,
	                              ComPtr<ID3D11PixelShader>& aPsToSet);
	static void CreateVertexShaderAndMaybeInputLayout(ID3D11Device* aDevice, const std::string& aShaderName,
	                                                  ComPtr<ID3D11VertexShader>& aVsToSet,
	                                                  ComPtr<ID3D11InputLayout>* aInputLayoutToCreate = nullptr);


	bool meshBandAid = false;
	// PORTFOLIO OBJECTS END //
};
