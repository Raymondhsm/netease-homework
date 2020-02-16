#include "pch.h"
#include "D3DApp.h"
#include "Utils/DirectXHelper.h"

namespace
{
	D3DApp* g_pd3dApp = nullptr;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_pd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}


D3DApp::D3DApp(HINSTANCE hInstance):
	m_hAppInst(hInstance),
	m_MainWndCaption(L"NetEase Job"),
	m_ClientWidth(800),
	m_ClientHeight(600),
	m_hMainWnd(nullptr),
	m_AppPaused(false),
	m_Minimized(false),
	m_Maximized(false),
	m_Resizing(false),
	m_Enable4xMsaa(true),
	m_4xMsaaQuality(0),
	m_pd3dDevice(nullptr),
	m_pd3dContext(nullptr),
	m_pSwapChain(nullptr),
	m_pDepthStencilBuffer(nullptr),
	m_pRenderTargetView(nullptr),
	m_pDepthStencilView(nullptr)
{
	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));
	g_pd3dApp = this;

	if(!Initialize())
		MessageBox(0, L"Initialize Failed!", 0, 0);
}

// 初始化创建窗口
bool D3DApp::CreateMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"D3DWndClassName",m_MainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}

bool D3DApp::CreateDeviceDependentResource()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#if defined(DUBUG) || defined(_DUBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DUBUG;
#endif

	// 创建驱动数组
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	// 创建特性等级数组
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	// 创建 Direct3D 11 API 设备对象和对应的上下文。
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	hr = D3D11CreateDevice(
		nullptr,					// 指定 nullptr 以使用默认适配器。
		D3D_DRIVER_TYPE_HARDWARE,	// 创建使用硬件图形驱动程序的设备。
		0,							// 应为 0，除非驱动程序是 D3D_DRIVER_TYPE_SOFTWARE。
		createDeviceFlags,			// 设置调试和 Direct2D 兼容性标志。
		featureLevels,				// 此应用程序可以支持的功能级别的列表。
		ARRAYSIZE(featureLevels),	// 上面的列表的大小。
		D3D11_SDK_VERSION,			// 对于 Windows 应用商店应用，始终将此值设置为 D3D11_SDK_VERSION。
		&device,					// 返回创建的 Direct3D 设备。
		&m_d3dFeatureLevel,			// 返回所创建设备的功能级别。
		&context					// 返回设备的即时上下文。
	);

	// 如果初始化失败，则回退到 WARP 设备。
	if (FAILED(hr))
	{
		ThrowIfFailed(
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // 创建 WARP 设备而不是硬件设备。
				0,
				createDeviceFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&device,
				&m_d3dFeatureLevel,
				&context
			)
		);
	}

	// 将指针存储到 Direct3D 11.3 API 设备和即时上下文中。
	ThrowIfFailed(device.As(&m_pd3dDevice));
	ThrowIfFailed(context.As(&m_pd3dContext));

	return true;
}

int D3DApp::Run()
{
	MSG msg = { 0 };

	m_Timer.ResetElapsedTime();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			

			if (!m_AppPaused)
			{
				m_Timer.Tick([&]()
				{
					Update();
				});
				if (Render())
				{
					Present();
				}
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

void D3DApp::HandleDeviceLost()
{
	m_pSwapChain = nullptr;

	OnDeviceLost();

	CreateDeviceDependentResource();
	CreateWindowSizeDependentResource();

	OnDeviceRestore();
}

bool D3DApp::Initialize()
{
	if (!(CreateMainWindow() && CreateDeviceDependentResource()))
		return false;
	CreateWindowSizeDependentResource();
	return true;
}

void D3DApp::CreateWindowSizeDependentResource()
{
	// 清除特定于上一窗口大小的上下文。
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_pd3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	m_pRenderTargetView = nullptr;
	m_pDepthStencilView = nullptr;
	m_pd3dContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

	// 如果交换链为空 创建
	if (m_pSwapChain == nullptr)
	{
		// 此序列获取用来创建上面的 Direct3D 设备的 DXGI 工厂。
		ComPtr<IDXGIDevice3> dxgiDevice;
		ComPtr<IDXGIAdapter> dxgiAdapter;
		ComPtr<IDXGIFactory4> dxgiFactory;

		ThrowIfFailed(
			m_pd3dDevice.As(&dxgiDevice), L"dxgiDevice failed"
		);

		ThrowIfFailed(
			dxgiDevice->GetAdapter(&dxgiAdapter), L"dxgiAdapter failed"
		);

		ThrowIfFailed(
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)), L"dxgiFactory failed"
		);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Height = m_ClientHeight;
		swapChainDesc.Width = m_ClientWidth;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc = { 0 };
		swapChainFullscreenDesc.RefreshRate.Numerator = 60;
		swapChainFullscreenDesc.RefreshRate.Denominator = 1;
		swapChainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainFullscreenDesc.Windowed = TRUE;

		ComPtr<IDXGISwapChain1> swapChain;
		ThrowIfFailed(
			dxgiFactory->CreateSwapChainForHwnd(
				m_pd3dDevice.Get(),
				m_hMainWnd,
				&swapChainDesc,
				&swapChainFullscreenDesc,
				nullptr,
				&swapChain
			)
		);
		ThrowIfFailed(
			swapChain.As(&m_pSwapChain)
		);

		ThrowIfFailed(
			dxgiDevice->SetMaximumFrameLatency(1)
		);
	}
	else
	{
		// 如果交换链已存在，请调整其大小。
		HRESULT hr = m_pSwapChain->ResizeBuffers(
			2, // 双缓冲交换链。
			lround(m_ClientWidth),
			lround(m_ClientHeight),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			// 如果出于任何原因移除了设备，将需要创建一个新的设备和交换链。
			HandleDeviceLost();

			return;
		}
		else
		{
			ThrowIfFailed(hr);
		}
	}

	// 创建交换链后台缓冲区的渲染目标视图。
	ComPtr<ID3D11Texture2D1> backBuffer;
	ThrowIfFailed(
		m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))
	);

	ThrowIfFailed(
		m_pd3dDevice->CreateRenderTargetView1(
			backBuffer.Get(),
			nullptr,
			&m_pRenderTargetView
		)
	);

	// 根据需要创建用于 3D 渲染的深度模具视图。
	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		lround(m_ClientWidth),
		lround(m_ClientHeight),
		1, // 此深度模具视图只有一个纹理。
		1, // 使用单一 mipmap 级别。
		D3D11_BIND_DEPTH_STENCIL
	);

	ThrowIfFailed(
		m_pd3dDevice->CreateTexture2D1(
			&depthStencilDesc,
			nullptr,
			&m_pDepthStencilBuffer
		)
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	ThrowIfFailed(
		m_pd3dDevice->CreateDepthStencilView(
			m_pDepthStencilBuffer.Get(),
			&depthStencilViewDesc,
			&m_pDepthStencilView
		)
	);

	// 设置用于确定整个窗口的 3D 渲染视区。
	m_ScreenViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		m_ClientWidth,
		m_ClientHeight
	);

	m_pd3dContext->RSSetViewports(1, &m_ScreenViewport);

}


LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_AppPaused = true;
		}
		else
		{
			m_AppPaused = false;
		}
		return 0;

	case WM_SIZE:
		m_ClientWidth = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		if (m_pd3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_AppPaused = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_AppPaused = false;
				m_Minimized = false;
				m_Maximized = true;
				CreateWindowSizeDependentResource();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_Minimized)
				{
					m_AppPaused = false;
					m_Minimized = false;
					CreateWindowSizeDependentResource();
				}
				else if (m_Maximized)
				{
					m_AppPaused = false;
					m_Maximized = false;
					CreateWindowSizeDependentResource();
				}
				else if (m_Resizing)
				{
					
				}
				else 
				{
					CreateWindowSizeDependentResource();
				}
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		m_AppPaused = true;
		m_Resizing = true;
		return 0;

	case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing = false;
		CreateWindowSizeDependentResource();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		OnInputEvent(msg, wParam, lParam);
		return 0;
	}


	return DefWindowProc(hwnd, msg, wParam, lParam);
}

HINSTANCE D3DApp::GetAppInst() const
{
	return m_hAppInst;
}

HWND D3DApp::GetMainWnd() const
{
	return m_hMainWnd;
}

float D3DApp::GetAspectRatio() const
{
	return static_cast<float>(m_ClientWidth) / m_ClientHeight;
}
