#include "GameController.h"
#include "Utils/DirectXHelper.h"
#include <sstream>

GameController::GameController(HINSTANCE hInstance):
	D3DApp(hInstance)
{
	// initialize function
}

void GameController::Update()
{
	std::wostringstream outs;
	outs.precision(6);
	outs << m_MainWndCaption << L"    " << L"FPS: " << m_Timer.GetFramesPerSecond();
	SetWindowText(m_hMainWnd, outs.str().c_str());
	//update function
}

bool GameController::Render()
{
	if (m_Timer.GetFrameCount() == 0)
		return false;

	m_pd3dContext->RSSetViewports(1, &m_ScreenViewport);

	// 将呈现目标重置为屏幕。
	ID3D11RenderTargetView *const targets[1] = { m_pRenderTargetView.Get() };
	m_pd3dContext->OMSetRenderTargets(1, targets, m_pDepthStencilView.Get());

	// 清除后台缓冲区和深度模具视图。
	m_pd3dContext->ClearRenderTargetView(m_pRenderTargetView.Get(), DirectX::Colors::CornflowerBlue);
	m_pd3dContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// render function

	return true;
}

void GameController::Present()
{
	HRESULT hr = m_pSwapChain->Present(1, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost();
	}
	else
	{
		DXHelper::ThrowIfFailed(hr);
	}
}

void GameController::OnDeviceLost()
{
	//release function
}

void GameController::OnDeviceRestore()
{
	// createDeviceDependent function


	CreateWindowSizeDependentResource();
}

void GameController::CreateWindowSizeDependentResource()
{
	D3DApp::CreateWindowSizeDependentResource();

	// CreateWindowSizeDependentResource function

}
