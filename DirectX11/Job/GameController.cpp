#include "pch.h"
#include "GameController.h"
#include "Utils/DirectXHelper.h"
#include <sstream>

GameController::GameController(HINSTANCE hInstance):
	D3DApp(hInstance)
{
	m_camera = std::shared_ptr<Job::Camera>(new Job::Camera());
	m_inputController = std::shared_ptr<InputController>(new InputController());

	// initialize function
	m_skyRenderer = std::unique_ptr<Job::SkyboxRender>(new Job::SkyboxRender(std::shared_ptr<D3DApp>(this), m_camera));
}

GameController::~GameController()
{
	m_skyRenderer.release();
}

void GameController::Update()
{
	std::wostringstream outs;
	outs.precision(6);
	outs << m_MainWndCaption << L"    " << L"FPS: " << m_Timer.GetFramesPerSecond();
	outs << L"     " << m_inputController->IsKeyPressed(InputController::A) << L"-" << m_inputController->IsKeyReleased(InputController::A);
	outs << L"    " << m_inputController->GetKeyState(InputController::A);
	SetWindowText(m_hMainWnd, outs.str().c_str());

	m_inputController->Update();
	m_camera->YawDegree(1);

	//update function
	m_skyRenderer->Update(m_Timer);
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
	m_skyRenderer->Render();

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
	m_skyRenderer->ReleaseDeviceDependentResources();
}

void GameController::OnDeviceRestore()
{
	// createDeviceDependent function
	m_skyRenderer->CreateDeviceDependentResources();

	CreateWindowSizeDependentResource();
}

void GameController::CreateWindowSizeDependentResource()
{
	D3DApp::CreateWindowSizeDependentResource();

	float aspectRatio = GetAspectRatio();
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}
	m_camera->UpdateFrustum(fovAngleY, aspectRatio, 0.01f, 1500.0f);

	// CreateWindowSizeDependentResource function
	m_skyRenderer->CreateWindowSizeDependentResources();
}

void GameController::OnInputEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
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
		m_inputController->ProcessMouseMessage(message, wParam, lParam);
		return;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		m_inputController->ProcessKeyboardMessage(message, wParam, lParam);
		return;
	}


	
}

