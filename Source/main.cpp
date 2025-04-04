// Preprocessor directives and definitions
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Conditional compilation for debugging and IMGUI
//#ifdef _DEBUG // imgui is only allowed in debug.
#define IMGUI
//#endif
#define USE_OPENEXR
#define REPORT_DX_WARNINGS

#include <Windows.h>
#include "GraphicsEngine.h"
#include "GlobalManager.h"
#include "imgui_Wrapper.h" // back from the dead 
#include "resource.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool InitializeWindow(HINSTANCE hInstance, int nCmdShow, HWND& hWnd, int aWidth, int aHeight);
// TODO- other initializer other stuff
void Cleanup(GraphicsEngine& graphicsEngine);
#ifdef IMGUI
void MessageLoop(HWND hWnd, GraphicsEngine& graphicsEngine, imgui_Wrapper& imgui);
#else
void MessageLoop(HWND hWnd, GraphicsEngine& graphicsEngine);
#endif

int width = 1920;
int height = 1080;

GraphicsEngine* graphicsEnginePtr = nullptr;

int APIENTRY wWinMain(
	_In_ const HINSTANCE hInstance,
	_In_opt_ HINSTANCE,
	_In_ LPWSTR,
	_In_ const int nCmdShow)
{
	HWND hWnd;
	//int width = 1280; // NOTE- THIS IS FOR THE ENTIRE WINDOW- INCLUDING THE WINDOWS BAR. NOT JUST THE BACKBUFFER / INSIDE SCREEN.
	//int height = 720;

	if (!InitializeWindow(hInstance, nCmdShow, hWnd, width, height))
	{
		return 1;
	}

	SetupGlobals(&hWnd);

	GraphicsEngine* graphicsEngine = new GraphicsEngine(); // init direct3d
	graphicsEnginePtr = graphicsEngine;
	if (!graphicsEngine->Initialize(hWnd))
	{
		return 1;
	}

#ifdef IMGUI
	imgui_Wrapper IMGUI_WRAPPER(*graphicsEngine);
	IMGUI_WRAPPER.Setup(hWnd);

	MessageLoop(hWnd, *graphicsEngine, IMGUI_WRAPPER);
	Cleanup(*graphicsEngine);
#else
	MessageLoop(hWnd, *graphicsEngine);
	Cleanup(*graphicsEngine);
#endif
	return 0;
}
// Slut på wWinMain

bool InitializeWindow(const HINSTANCE hInstance, const int nCmdShow, HWND& hWnd, const int aWidth, const int aHeight)
{
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;

	wcex.hCursor = LoadCursor(nullptr, IDC_ICON); // picks our cursor (the 2nd)
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);

	auto customIcon = LoadIcon(hInstance, MAKEINTRESOURCE(FOLKE_ICON));
	if (!customIcon)
	{
		MessageBox(nullptr, LPCWSTR("Error code: " + GetLastError()), L"ICON IS MISSING FROM GAME!", MB_ICONERROR | MB_HELP);
		return false;
	}
	wcex.hIcon = customIcon;
	wcex.hIconSm = customIcon;
	constexpr wchar_t windowName[] = L"momoEngine"; // registrerar vårt namn till vårt fönster, note the use of square brackets for wchar_t, it being an array
	wcex.lpszClassName = windowName;

	if (!RegisterClassExW(&wcex))
	{
		return false;
	}

	hWnd = CreateWindow(windowName, windowName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, aWidth, aHeight, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		return false;
	}

	// Måste också kalla på funktionen för att visa och uppdatera vårt fönster
	ShowWindow(hWnd, nCmdShow); // this can be wherever, before or after imgui.
	UpdateWindow(hWnd);

	return true;
}

#ifdef IMGUI
void MessageLoop([[maybe_unused]] HWND hWnd, GraphicsEngine& graphicsEngine, imgui_Wrapper& imgui)
#else
void MessageLoop(HWND hWnd, GraphicsEngine& graphicsEngine)
#endif
{
	hWnd;
	// Main message loop:
	MSG msg = {};
	while (true)
	{
		// windows är väldigt meddelande drivande, så PeekMessage kollar egentligen att så länge vi har meddelande så hanterar vi dem också och den kallar winProc i sun tur senare.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				return;
			}
		}

		if (globalInput->GetIsKeyDown(KeyCode::Escape)) // shitty way to close that should be removed and dealt with properly. TODO
		{
			PostQuitMessage(0);
		}
		UpdateGlobals();
		graphicsEngine.Update(globalTimer->GetDeltaTime());
#ifdef IMGUI
		imgui_Wrapper::NewFrame();
		imgui.ADD_STUFF_HERE(graphicsEngine);
		imgui.Render();
#endif
		graphicsEngine.Render(); // clenched between imgui
#ifdef IMGUI
		graphicsEngine.myDX11Annotation.BeginAnnotation(L"ImGui");
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		graphicsEngine.myDX11Annotation.EndAnnotation();
#endif
		if (!graphicsEngine.Present())
		{
			return;
		}
	}
}

void Cleanup(GraphicsEngine& graphicsEngine)
{
#ifdef IMGUI
	imgui_Wrapper::CleanUp();
#endif
	DeleteGlobals();
	delete& graphicsEngine;
}

#ifdef IMGUI
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT CALLBACK WndProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	globalInput->UpdateEvents(msg, wParam, lParam);
#ifdef IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
#endif

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (graphicsEnginePtr)
		{

			width = LOWORD(lParam);
			height = HIWORD(lParam);

			if (width == 0 || height == 0)
			{
				return 0; // Skip resizing when minimized
			}

			auto& graphicsEngine = *graphicsEnginePtr;
			auto& context = graphicsEngine.myContext;
			//auto& swapChain = graphicsEngine.mySwapChain;
			//auto& device = graphicsEngine.myDevice;

			context->OMSetRenderTargets(0, nullptr, nullptr);
			graphicsEngine.myBackBuffer->Release();
			graphicsEngine.myBackBufferTexture->Release();
			graphicsEngine.myDepthBufferNew.DSV->Release();
			graphicsEngine.myDepthBufferNew.SRV->Release();
			graphicsEngine.myDepthBufferNew.Tex->Release();

			HRESULT hr;

			if (PortfolioSettings::vsync)
			{
				hr = graphicsEngine.mySwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

			}
			else
			{
				hr = graphicsEngine.mySwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
			}
			if (FAILED(hr))
			{
				throw;
			}


			graphicsEngine.CreateBackBuffer(hr);
			graphicsEngine.CreateDepthBuffer(hr);

			D3D11_VIEWPORT vp;
			vp.Width = static_cast<float>(width);
			vp.Height = static_cast<float>(height);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			context->RSSetViewports(1, &vp);
			//hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			//	reinterpret_cast<void**>(graphicsEngine.myBackBufferTexture.GetAddressOf()));
			//if (FAILED(hr))
			//{
			//	throw;
			//}

			//hr = device->CreateRenderTargetView(graphicsEngine.myBackBufferTexture.Get(), NULL, graphicsEngine.myBackBuffer.GetAddressOf());
			//if (FAILED(hr))
			//{
			//	throw;
			//}

			//context->OMSetRenderTargets(1, graphicsEngine.myBackBuffer.GetAddressOf(), NULL);
		}
		//#ifdef IMGUI
		//	case WM_DPICHANGED:
		//		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		//		{
		//			const int dpi = HIWORD(wParam);
		//			printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
		//			const RECT* suggested_rect = (RECT*)lParam;
		//			SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		//
		//		}
		//		break;
		//#endif
				// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-activate?redirectedfrom=MSDN
				// https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-killfocus
				// https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-activateapp
			//case WM_ACTIVATEAPP: // a different window has gained focus. TODO- add mouse release func here.
			//	globalInput->ReleaseCursorFromWindow();
			//	break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	//return 0;
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}