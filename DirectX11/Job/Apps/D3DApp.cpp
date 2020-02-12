#include "../pch.h"
#include "D3DApp.h"
#include "../Utils/DirectXHelper.h"

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
	//CreateDeviceIndependentResources();
	//CreateDeviceResources();
}

// 初始化创建窗口
bool D3DApp::InitMainWindow()
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

	m_hMainWnd = CreateWindow(L"D3DWndClassName", m_MainWndCaption.c_str(),
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

bool D3DApp::InitDirect3D()
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
		DXHelper::ThrowIfFailed(
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
	DXHelper::ThrowIfFailed(device.As(&m_pd3dDevice));
	DXHelper::ThrowIfFailed(context.As(&m_pd3dContext));

	return true;
}

void D3DApp::UpdateRenderTargetSize()
{
	return;
}

void D3DApp::HandleDeviceLost()
{
	return;
}

int D3DApp::Run()
{
	return 0;
}

bool D3DApp::Initialize()
{
	return InitMainWindow() && InitDirect3D();
}

void D3DApp::CreateWindowSizeDependentResource()
{
	UpdateRenderTargetSize();

	// 如果交换链为空 创建
	if (m_pSwapChain == nullptr)
	{
		// 此序列获取用来创建上面的 Direct3D 设备的 DXGI 工厂。
		ComPtr<IDXGIDevice3> dxgiDevice;
		ComPtr<IDXGIAdapter> dxgiAdapter;
		ComPtr<IDXGIFactory4> dxgiFactory;

		DXHelper::ThrowIfFailed(
			m_pd3dDevice.As(&dxgiDevice), L"dxgiDevice failed"
		);

		DXHelper::ThrowIfFailed(
			dxgiDevice->GetAdapter(&dxgiAdapter), L"dxgiAdapter failed"
		);

		DXHelper::ThrowIfFailed(
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
		DXHelper::ThrowIfFailed(
			dxgiFactory->CreateSwapChainForHwnd(
				m_pd3dDevice.Get(),
				m_hMainWnd,
				&swapChainDesc,
				&swapChainFullscreenDesc,
				nullptr,
				&swapChain
			)
		);
		DXHelper::ThrowIfFailed(
			swapChain.As(&m_pSwapChain)
		);

		DXHelper::ThrowIfFailed(
			dxgiDevice->SetMaximumFrameLatency(1)
		);

		// 创建交换链后台缓冲区的渲染目标视图。
		ComPtr<ID3D11Texture2D1> backBuffer;
		DXHelper::ThrowIfFailed(
			m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))
		);

		DXHelper::ThrowIfFailed(
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

		DXHelper::ThrowIfFailed(
			m_pd3dDevice->CreateTexture2D1(
				&depthStencilDesc,
				nullptr,
				&m_pDepthStencilBuffer
			)
		);

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
		DXHelper::ThrowIfFailed(
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
			DXHelper::ThrowIfFailed(hr);
		}
	}

}

bool D3DApp::CreateDeviceDependentResource()
{
	return false;
}

bool D3DApp::Update()
{
	return false;
}

bool D3DApp::Render()
{
	return false;
}

bool D3DApp::Present()
{
	return false;
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

HINSTANCE D3DApp::GetAppInst() const
{
	return HINSTANCE();
}

HWND D3DApp::GetMainWnd() const
{
	return HWND();
}

float D3DApp::GetAspectRatio() const
{
	return 0.0f;
}
