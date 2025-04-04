#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h" // used for docker stuff!
#include "GraphicsEngine.h" // TODO- MOVE TO CPP!
#include "PortfolioSettings.h"

class imgui_Wrapper
{
public:
	// this is where you add any imgui stuff you want. DO NOT STORE VARIABLES HERE. THIS IS IMGUI UPDATE, ESSENTIALLY. 
	void ADD_STUFF_HERE(GraphicsEngine& aGraphicsEngine);

	// setup functions, ignore these.
	void Setup(HWND aHwnd); // joint function for indented
	void InitImgui();
	void SetFlags() const;
	void SetupPlatformRendererBackends(HWND aHwnd) const;
	void LoadFonts(); // these guys
	static void NewFrame();
	void Render() const;
	static void CleanUp();
	imgui_Wrapper(GraphicsEngine& aGraphicsEngine) : myGraphicsEngine(aGraphicsEngine) {}

	void RenderTexture2D(ID3D11ShaderResourceView* aSRV, ImVec1 aScale);
	void RenderTexture2D(ID3D11ShaderResourceView* aSRV, ImVec2 aXYScale);
	void RenderTexture3D(ID3D11ShaderResourceView* aSRV, CommonUtilities::Vector2f aScale, UINT sliceIndex, GraphicsEngine& aGraphicsEngine);

	// bandaid, remove.
	void RenderTexture3DCurl(ID3D11ShaderResourceView* aSRV, CommonUtilities::Vector2f aScale, UINT sliceIndex, GraphicsEngine& aGraphicsEngine);
private:
	ImGuiIO* myIO = nullptr;
	GraphicsEngine& myGraphicsEngine;

	bool showImguiDemoWindow = true; // so fkn stupid

	ID3D11Texture2D* pSliceTextureWorley; // Persistent 2D texture
	ID3D11ShaderResourceView* pSliceSRVWorley; // Persistent SRV

	ID3D11Texture2D* pSliceTextureCurl; // Persistent 2D texture
	ID3D11ShaderResourceView* pSliceSRVCurl; // Persistent SRV
};

// aids for imgui
namespace imguiAids
{
	// Function to create the dropdown
	inline bool DrawPhaseFunctionDropdown(const char* label, RayMarcherStructs::PhaseFunction& currentPhase)
	{
		// Array of display names for the dropdown
		static const char* phaseNames[] = {
			"Henyey-Greenstein",
			"Mie",
			"Rayleigh"
		};

		// Convert current enum to int for ImGui
		int currentItem = static_cast<int>(currentPhase);

		// Create the combo box
		bool changed = ImGui::Combo(label, &currentItem, phaseNames, IM_ARRAYSIZE(phaseNames));

		// Update the enum value if changed
		if (changed) {
			currentPhase = static_cast<RayMarcherStructs::PhaseFunction>(currentItem);
		}

		return changed;
	}

	inline bool RenderSizeDropdown(const char* label, RayMarcherStructs::Res& currentRes)
	{
		// Array of display names for the dropdown
		static const char* phaseNames[] = {
			"Full",
			"Half",
			"Quarter"
		};

		// Convert current enum to int for ImGui
		int currentItem = static_cast<int>(currentRes);

		// Create the combo box
		bool changed = ImGui::Combo(label, &currentItem, phaseNames, IM_ARRAYSIZE(phaseNames));

		// Update the enum value if changed
		if (changed) {
			currentRes = static_cast<RayMarcherStructs::Res>(currentItem);
		}

		return changed;
	}
}

inline void imgui_Wrapper::ADD_STUFF_HERE([[maybe_unused]] GraphicsEngine& aGraphicsEngine) // DO NOT STORE VARIABLES HERE. THIS IS IMGUI UPDATE, ESSENTIALLY. 
{
	// show demo window
	//ImGui::ShowDemoWindow(&showImguiDemoWindow);
	//myGraphicsEngine.myTriangle.myVertices[0].x;
	//ImGui::SliderFloat("test", &myGraphicsEngine.myTriangle.myVertices[0].x, -1.0f, 1.0f, "ratio = %.3f");
	auto cubePosTemp = aGraphicsEngine.MyNormalCube.GetPositionXMFLOAT3();
	//auto cubeScaleTemp = aGraphicsEngine.MyNormalAssCube.GetScale();

	ImGui::Begin("stuff");
	//ImGui::SliderFloat("vert", &aGraphicsEngine.myCamera.myVerticalFOV, 0, 360);
	//ImGui::SliderFloat("hor", &aGraphicsEngine.myCamera.myHorizontalFOV, 0, 360);

	if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImVec1 winSize = ImGui::GetContentRegionAvail().x;
		ImVec1 targetWidth1stRow = { winSize.x / 2 };

		ImGui::Text("Smoke Albedo:");
		ImGui::SameLine(winSize.x / 2);
		ImGui::Text("Smoke Mask:");
		RenderTexture2D(aGraphicsEngine.myRayMarcher.GetSmokeAlbedoTex().srv.Get(), targetWidth1stRow);
		ImGui::SameLine();
		RenderTexture2D(aGraphicsEngine.myRayMarcher.GetSmokeMaskTex().srv.Get(), targetWidth1stRow);


		ImVec1 targetWidth2ndRow = { winSize.x / 3 };

		// debug tex's
		ImGui::Text("Debug Textures: (Density, Trilinear Voxels, Noise)");
		RenderTexture2D(aGraphicsEngine.myRayMarcher.debugTex1.srv.Get(), targetWidth2ndRow);
		ImGui::SameLine();
		RenderTexture2D(aGraphicsEngine.myRayMarcher.debugTex2.srv.Get(), targetWidth2ndRow);
		ImGui::SameLine();
		RenderTexture2D(aGraphicsEngine.myRayMarcher.debugTex3.srv.Get(), targetWidth2ndRow);

		ImGui::Text("Smoke Noise (3D Tex)");
		RenderTexture3D(aGraphicsEngine.myRayMarcher.noiseTex.srv.Get(), { PortfolioSettings::ImGuiNoiseTexSize, PortfolioSettings::ImGuiNoiseTexSize }, PortfolioSettings::debugWorleyNoiseSlice, aGraphicsEngine);
		ImGui::DragInt("3D Slice (Depth)", &PortfolioSettings::debugWorleyNoiseSlice, 1, 0, 127);
		ImGui::Spacing();
		ImGui::Spacing();

		/*ImGui::Text("Smoke Curl Noise (3D Tex)");
		RenderTexture3DCurl(aGraphicsEngine.myRayMarcher.curlTex.srv.Get(), { 1.f, 1.f }, PortfolioSettings::debugCurlNoiseSlice, aGraphicsEngine);
		ImGui::DragInt("3D Curl Slice (Depth)", &PortfolioSettings::debugCurlNoiseSlice, 1, 0, 127);
		ImGui::Spacing();
		ImGui::Spacing();*/
		//ImGui::DragInt("Blend Function (Breaks some stuff)", &PortfolioSettings::debugView, .01f, 0, 2);
		//ImGui::DragFloat("bi strength", &PortfolioSettings::biTagentNoiseStrength, .01f, 0.f, 100.f);
		//ImGui::DragFloat("bi scale", &PortfolioSettings::biTagentNoiseScale, .01f, -100.f, 100.f);
	}
	if (ImGui::CollapsingHeader("Settings"))
	{
		ImGui::Text("Sun");
		ImGui::DragFloat3("Sun Position", reinterpret_cast<float*>(&PortfolioSettings::sun.pos), .25f, -100, 100);
		ImGui::DragFloat3("Sun Color", reinterpret_cast<float*>(&PortfolioSettings::sun.color), .01f, -100, 100);

		ImGui::Spacing();
		ImGui::DragFloat3("World BG Color", reinterpret_cast<float*>(&aGraphicsEngine.myBGColor), .01f, 0, 100);
		//ImGui::Checkbox("Vsync", &PortfolioSettings::vsync);
		ImGui::Spacing();

		//ImGui::PushItemWidth(80);
		//ImGui::ColorPicker3("Sun Color", reinterpret_cast<float*>(&PortfolioSettings::sun.color));

		if (ImGui::CollapsingHeader("Smoke Settings"))
		{
			imguiAids::RenderSizeDropdown("Render Size", PortfolioSettings::resScale);
			ImGui::Checkbox("Enable ToneMap", &PortfolioSettings::activateTonemap);
			ImGui::DragFloat3("Smoke Color", reinterpret_cast<float*>(&PortfolioSettings::smoke.color), .01f, -100, 100);
			ImGui::DragFloat("Mask Influence", &PortfolioSettings::maskInfluence, .01f, 0.01f, 20.f);
			ImGui::DragFloat3("Animation Direction", reinterpret_cast<float*>(&PortfolioSettings::animDirection), .01f, -100, 100);
			ImGui::DragInt("Step Count", &PortfolioSettings::stepCount, .1f, 1, 256);
			ImGui::DragFloat("Step Size", &PortfolioSettings::stepSize, .001f, 0.01f, 1.f);
			ImGui::DragInt("Light Step Count", &PortfolioSettings::lightStepCount, .01f, 1, 32);
			ImGui::DragFloat("Light Step Size", &PortfolioSettings::lightStepSize, .01f, 0.01f, 1.f);
			ImGui::DragFloat("Smoke Size", &PortfolioSettings::smokeSize, .01f, 0.01f, 64.f);
			ImGui::DragFloat("Volume Density", &PortfolioSettings::volumeDensity, .01f, 0.0f, 10.f);
			ImGui::DragFloat("Absorption Coefficient", &PortfolioSettings::absorptionCoefficient, .01f, 0.f, 10.f);
			ImGui::DragFloat("Scattering Coefficient", &PortfolioSettings::scatteringCoefficient, .01f, 0.f, 10.f);
			ImGui::DragFloat3("Extinction Color", reinterpret_cast<float*>(&PortfolioSettings::extinctionColor), .01f, -100, 100);
			ImGui::DragFloat("Shadow Density", &PortfolioSettings::shadowDensity, 0.1f, 0.f, 2.f);
			ImGui::DragFloat("Scattering Anisotropy", &PortfolioSettings::scatteringAnisotropy, .005f, -1.f, 1.f);
			ImGui::DragFloat("Density Falloff", &PortfolioSettings::densityFalloff, .01f, 0.f, 1.f);
			ImGui::DragFloat("Alpha Threshold", &PortfolioSettings::alphaThreshold, .01f, 0.f, 1.f);
			//ImGui::DragFloat("Sharpness", &PortfolioSettings::sharpness, .01f, -100.f, 100.f);
			imguiAids::DrawPhaseFunctionDropdown("phase functions", PortfolioSettings::phaseFunc);
			ImGui::DragFloat("Voxel Growth Steepness", &PortfolioSettings::voxelGrowthSteepness, .01f, -5.f, 5.f);
			ImGui::DragFloat("Raymarch Growth Steepness", &PortfolioSettings::rayMarchGrowthSteepness, .01f, -5.f, 5.f);
			ImGui::DragFloat("Gradient Noise Strength", &PortfolioSettings::InterleavedGradientNoiseStrength, .01f, -20.f, 20.f);
		}
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Voxel Settings"))
		{
			ImGui::Checkbox("Draw Voxels? (skips the drawInstanced, cs still runs)", &PortfolioSettings::drawVoxels);
			ImGui::Checkbox("Draw Smoke Voxels?", &PortfolioSettings::drawSmokeVoxels);
			ImGui::Checkbox("Draw Smoke Seed?", &PortfolioSettings::drawSmokeSeed);
			ImGui::Checkbox("Draw Static Voxels?", &PortfolioSettings::drawStaticVoxels);
			ImGui::Checkbox("Draw Edge Voxels?", &PortfolioSettings::drawEdgeVoxels);
			ImGui::Checkbox("Draw RayCast Cube?", &PortfolioSettings::drawRayCube);
			ImGui::DragFloat3("SmokeSize", reinterpret_cast<float*>(&PortfolioSettings::smoke.radius), .25f, 0, 10);

			ImGui::DragFloat3("Cube Position", reinterpret_cast<float*>(&cubePosTemp), .1f, -10, 10);
			//ImGui::DragFloat3("Cube Scale", reinterpret_cast<float*>(&cubeScaleTemp), .25f, 0, 10);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.0f, 1.0f)); // Green button
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); // Brighter orange on hover
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.3f, 0.0f, 1.0f)); // Darker orange when clicked
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f); // Rounded corners

			if (ImGui::Button("Voxelize Meshes", { 125,25 }))
			{
				aGraphicsEngine.VoxelizeMeshes();
			}

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar();

			const char* label = PortfolioSettings::wireframe ? "Switch to Solid?" : "Switch to WireFrame?";
			if (ImGui::Button(label, { 155,25 }))
			{
				aGraphicsEngine.ToggleWireFrame();
			}
		}
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Noise Settings"))
		{
			ImGui::Checkbox("update noise", &PortfolioSettings::updateNoise);
			ImGui::DragInt("seed", &PortfolioSettings::seed, .25f, 0, 100000);
			ImGui::DragInt("Octaves", &PortfolioSettings::octaves, 1, 1, 16);
			ImGui::DragInt("Cell Size", &PortfolioSettings::cellSize, 1, 1, 128);
			ImGui::DragInt("Axis Cell Count", &PortfolioSettings::axisCellCount, 1, 1, 64);
			ImGui::DragFloat("Amplitude", &PortfolioSettings::amplitude, .25f, 0.1f, 16.f);
			ImGui::DragFloat("Warp", &PortfolioSettings::warp, 1.f, 0.f, 5.f);
			ImGui::DragFloat("Add", &PortfolioSettings::add, 1.f, -5.f, 5.f);
			ImGui::Checkbox("invert noise", &PortfolioSettings::invertNoise);
			//ImGui::DragFloat("ImGui Noise Tex Size", &PortfolioSettings::ImGuiNoiseTexSize, .25f, 0.1f, 50.f);
		}
		ImGui::Spacing();
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("CPU Timings"))
	{
		if (ImGui::BeginTable("CPU Timings", 2)) // Use 2 columns for key-value pairs
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("CPU TotalSeconds:");
			ImGui::TableNextColumn();
			ImGui::Text("%.6f", globalTimer->GetTotalSeconds());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("CPU DeltaTime:");
			ImGui::TableNextColumn();
			ImGui::Text("%.6f", globalTimer->GetDeltaTime());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("CPU Unscaled Total Seconds:");
			ImGui::TableNextColumn();
			ImGui::Text("%.6f", globalTimer->GetUnscaledTotalSeconds());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("CPU Unscaled Delta Time:");
			ImGui::TableNextColumn();
			ImGui::Text("%.6f", globalTimer->GetUnscaledDeltaTime());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("CPU Fixed DeltaTime:");
			ImGui::TableNextColumn();
			ImGui::Text("%.6f", globalTimer->GetFixedDeltaTime());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("CPU Fixed Total Seconds:");
			ImGui::TableNextColumn();
			ImGui::Text("%.6f", globalTimer->GetFixedTotalSeconds());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("CPU TimeScale:");
			ImGui::TableNextColumn();
			ImGui::Text("%.6f", globalTimer->GetTimeScale());

			ImGui::EndTable();
		}
	}
	aGraphicsEngine.MyNormalCube.SetPositionXMFLOAT3(cubePosTemp);
	//aGraphicsEngine.MyNormalAssCube.SetScale(cubeScaleTemp);

	if (ImGui::CollapsingHeader("GPU Timings"))
	{

		if (ImGui::BeginTable("GPU Timings", 2)) // Use 2 columns for key-value pairs
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("GPU FrameTime:");
			ImGui::TableNextColumn();
			ImGui::Text("%.3f ms", aGraphicsEngine.myDx11Timer.GetDeltaTime() * 1000.0f);

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("GPU FPS:");
			ImGui::TableNextColumn();
			ImGui::Text("%.1f", aGraphicsEngine.myDx11Timer.GetFPS());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("Present Count:");
			ImGui::TableNextColumn();
			ImGui::Text("%llu", aGraphicsEngine.myStats.PresentCount); // Assuming it's an integer

			ImGui::EndTable();
		}
	}

	ImGui::End();


	// IMGUI EXAMPLE.
	// window that takes over the entire background! sweet waooowie
	//{
	//	ImGui::DockSpaceOverViewport();
	//	ImGui::Begin("Hello, world!");
	//	ImGui::DockBuilderDockWindow("Hello, World!", ImGui::GetID("Hello, World!"));
	//	ImGui::End();
	//}
}

inline void imgui_Wrapper::Setup(const HWND aHwnd)
{
	InitImgui();
	SetFlags();
	SetupPlatformRendererBackends(aHwnd);
	LoadFonts();
}

inline void imgui_Wrapper::InitImgui()
{
	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext(); // todo- use context?
	myIO = &ImGui::GetIO();
	(void)myIO;
}

inline void imgui_Wrapper::SetFlags() const
{
	myIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	myIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	myIO->ConfigFlags |= ImGuiConfigFlags_IsSRGB;
	//myIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	//myIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	//myIO->ConfigFlags |= ImGuiDockNodeFlags_NoDockingInCentralNode; // Enable Multi-Viewport / Platform Windows
	//myIO->ConfigFlags |= ImGuiDockNodeFlags_PassthruCentralNode; // Enable Multi-Viewport / Platform Windows

	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//myIO->ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	//ImGuiStyle& style = ImGui::GetStyle();
	//if (myIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	style.WindowRounding = 0.0f;
	//	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	//}
}

inline void imgui_Wrapper::SetupPlatformRendererBackends(const HWND aHwnd) const
{
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(aHwnd);
	//ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	ImGui_ImplDX11_Init(myGraphicsEngine.GetDevice().Get(), myGraphicsEngine.GetContext().Get());
}

inline void imgui_Wrapper::LoadFonts()
{
	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);
}

inline void imgui_Wrapper::NewFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

inline void imgui_Wrapper::Render() const
{
	// RENDERING
	ImGui::Render(); // the order of imgui rendering should be looked at. the example does it before dx11.

	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//// Update and Render additional Platform Windows
	//if (myIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	ImGui::UpdatePlatformWindows();
	//	ImGui::RenderPlatformWindowsDefault();
	//}
}

inline void imgui_Wrapper::CleanUp()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

inline void imgui_Wrapper::RenderTexture2D(ID3D11ShaderResourceView* aSRV, ImVec1 aScale)
{
	ID3D11Resource* pResource = nullptr;
	aSRV->GetResource(&pResource);
	ID3D11Texture2D* pTexture = nullptr;
	pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
	D3D11_TEXTURE2D_DESC texDesc;
	pTexture->GetDesc(&texDesc);

	ImVec2 size = { static_cast<const float&>(texDesc.Width), static_cast<const float&>(texDesc.Height) };
	float scaleFactor = aScale.x / size.x;

	ImGui::Image((ImTextureID)(intptr_t)aSRV, ImVec2(size.x * scaleFactor, size.y * scaleFactor));
}

inline void imgui_Wrapper::RenderTexture2D(ID3D11ShaderResourceView* aSRV, ImVec2 aXYScale)
{
	ID3D11Resource* pResource = nullptr;
	aSRV->GetResource(&pResource);
	ID3D11Texture2D* pTexture = nullptr;
	pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);
	D3D11_TEXTURE2D_DESC texDesc;
	pTexture->GetDesc(&texDesc);

	ImVec2 size = { static_cast<const float&>(texDesc.Width), static_cast<const float&>(texDesc.Height) };

	ImGui::Image((ImTextureID)(intptr_t)aSRV, ImVec2(size.x * aXYScale.x, size.y * aXYScale.y));
}

inline void imgui_Wrapper::RenderTexture3D(ID3D11ShaderResourceView* aSRV, CommonUtilities::Vector2f aScale, UINT sliceIndex, GraphicsEngine& aGraphicsEngine)
{
	ID3D11Resource* pResource = nullptr;
	aSRV->GetResource(&pResource);
	ID3D11Texture3D* pTexture3D = nullptr;
	pResource->QueryInterface(__uuidof(ID3D11Texture3D), (void**)&pTexture3D);
	if (!pTexture3D)
	{
		ImGui::Text("Failed to get 3D texture.");
		pResource->Release();
		return;
	}

	D3D11_TEXTURE3D_DESC texDesc;
	pTexture3D->GetDesc(&texDesc);

	if (sliceIndex >= texDesc.Depth)
	{
		ImGui::Text("Invalid slice index.");
		pTexture3D->Release();
		pResource->Release();
		return;
	}

	// Create the 2D texture and SRV only once
	if (!pSliceTextureWorley)
	{
		D3D11_TEXTURE2D_DESC tex2DDesc = {};
		tex2DDesc.Width = texDesc.Width;
		tex2DDesc.Height = texDesc.Height;
		tex2DDesc.MipLevels = 1;
		tex2DDesc.ArraySize = 1;
		tex2DDesc.Format = texDesc.Format;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		HRESULT hr = aGraphicsEngine.GetDevice()->CreateTexture2D(&tex2DDesc, nullptr, &pSliceTextureWorley);
		if (FAILED(hr))
		{
			ImGui::Text("Failed to create 2D texture.");
			pTexture3D->Release();
			pResource->Release();
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = aGraphicsEngine.GetDevice()->CreateShaderResourceView(pSliceTextureWorley, &srvDesc, &pSliceSRVWorley);
		if (FAILED(hr))
		{
			ImGui::Text("Failed to create SRV.");
			pSliceTextureWorley->Release();
			pSliceTextureWorley = nullptr;
			pTexture3D->Release();
			pResource->Release();
			return;
		}
	}

	D3D11_TEXTURE3D_DESC texDesc1;
	pTexture3D->GetDesc(&texDesc1);

	D3D11_TEXTURE2D_DESC texDesc2;
	pSliceTextureWorley->GetDesc(&texDesc2);
	// Update the slice
	D3D11_BOX srcBox = { 0, 0, sliceIndex, texDesc.Width, texDesc.Height, sliceIndex + 1 };
	aGraphicsEngine.GetContext()->CopySubresourceRegion(pSliceTextureWorley, 0, 0, 0, 0, pTexture3D, 0, &srcBox);

	// Display in ImGui
	CommonUtilities::Vector2f size = { static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height) };
	ImGui::Image((ImTextureID)(intptr_t)pSliceSRVWorley, ImVec2(size.x * aScale.x, size.y * aScale.y));

	// Clean up temporary resources
	pTexture3D->Release();
	pResource->Release();
}

inline void imgui_Wrapper::RenderTexture3DCurl(ID3D11ShaderResourceView* aSRV, CommonUtilities::Vector2f aScale,
	UINT sliceIndex, GraphicsEngine& aGraphicsEngine)
{
	ID3D11Resource* pResource = nullptr;
	aSRV->GetResource(&pResource);
	ID3D11Texture3D* pTexture3D = nullptr;
	pResource->QueryInterface(__uuidof(ID3D11Texture3D), (void**)&pTexture3D);
	if (!pTexture3D)
	{
		ImGui::Text("Failed to get 3D texture.");
		pResource->Release();
		return;
	}

	D3D11_TEXTURE3D_DESC texDesc;
	pTexture3D->GetDesc(&texDesc);

	if (sliceIndex >= texDesc.Depth)
	{
		ImGui::Text("Invalid slice index.");
		pTexture3D->Release();
		pResource->Release();
		return;
	}

	// Create the 2D texture and SRV only once
	if (!pSliceTextureCurl)
	{
		D3D11_TEXTURE2D_DESC tex2DDesc = {};
		tex2DDesc.Width = texDesc.Width;
		tex2DDesc.Height = texDesc.Height;
		tex2DDesc.MipLevels = 1;
		tex2DDesc.ArraySize = 1;
		tex2DDesc.Format = texDesc.Format;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		HRESULT hr = aGraphicsEngine.GetDevice()->CreateTexture2D(&tex2DDesc, nullptr, &pSliceTextureCurl);
		if (FAILED(hr))
		{
			ImGui::Text("Failed to create 2D texture.");
			pTexture3D->Release();
			pResource->Release();
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = aGraphicsEngine.GetDevice()->CreateShaderResourceView(pSliceTextureCurl, &srvDesc, &pSliceSRVCurl);
		if (FAILED(hr))
		{
			ImGui::Text("Failed to create SRV.");
			pSliceTextureCurl->Release();
			pSliceTextureCurl = nullptr;
			pTexture3D->Release();
			pResource->Release();
			return;
		}
	}

	D3D11_TEXTURE3D_DESC texDesc1;
	pTexture3D->GetDesc(&texDesc1);

	D3D11_TEXTURE2D_DESC texDesc2;
	pSliceTextureCurl->GetDesc(&texDesc2);
	// Update the slice
	D3D11_BOX srcBox = { 0, 0, sliceIndex, texDesc.Width, texDesc.Height, sliceIndex + 1 };
	aGraphicsEngine.GetContext()->CopySubresourceRegion(pSliceTextureCurl, 0, 0, 0, 0, pTexture3D, 0, &srcBox);

	// Display in ImGui
	CommonUtilities::Vector2f size = { static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height) };
	ImGui::Image((ImTextureID)(intptr_t)pSliceSRVCurl, ImVec2(size.x * aScale.x, size.y * aScale.y));

	// Clean up temporary resources
	pTexture3D->Release();
	pResource->Release();
}
