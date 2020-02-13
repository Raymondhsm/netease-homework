#include "GameController.h"

GameController::GameController(HINSTANCE hInstance):
	D3DApp(hInstance)
{
}

void GameController::Update()
{
}

bool GameController::Render()
{
	if (m_Timer.GetFrameCount() == 0)
		return false;

	static float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };	// RGBA = (0,0,255,255)
	m_pd3dContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blue);
	m_pd3dContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return true;
}

void GameController::Present()
{
	m_pSwapChain->Present(0, 0);
}

void GameController::CreateWindowSizeDependentResource()
{
}
