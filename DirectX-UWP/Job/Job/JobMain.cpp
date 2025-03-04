﻿#include "pch.h"
#include "JobMain.h"
#include "Common\DirectXHelper.h"

using namespace Job;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// 加载应用程序时加载并初始化应用程序资产。
JobMain::JobMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	// 注册以在设备丢失或重新创建时收到通知
	m_deviceResources->RegisterDeviceNotify(this);

	m_camera = std::shared_ptr<Camera>(new Camera());
	m_inputController = ref new InputController();

	// TODO: 将此替换为应用程序内容的初始化。
	//m_carRenderer = std::unique_ptr<CarRenderer>(new CarRenderer(m_deviceResources));

	m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(m_deviceResources));

	m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(m_deviceResources));

	m_skyRenderer = std::unique_ptr<SkyboxRender>(new SkyboxRender(m_deviceResources, m_camera));

	// TODO: 如果需要默认的可变时间步长模式之外的其他模式，请更改计时器设置。
	// 例如，对于 60 FPS 固定时间步长更新逻辑，请调用:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

JobMain::~JobMain()
{
	// 取消注册设备通知
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// 在窗口大小更改(例如，设备方向更改)时更新应用程序状态
void JobMain::CreateWindowSizeDependentResources() 
{
	// TODO: 将此替换为应用程序内容的与大小相关的初始化。
	m_sceneRenderer->CreateWindowSizeDependentResources();
	//m_carRenderer->CreateWindowSizeDependentResources();
	m_skyRenderer->CreateWindowSizeDependentResources();
}

// 每帧更新一次应用程序状态。
void JobMain::Update() 
{
	// 更新场景对象。
	m_timer.Tick([&]()
	{
		if (m_inputController->isLeft()) m_camera->YawDegree(1);
		else if (m_inputController->isRight()) m_camera->YawDegree(-1);
		else if (m_inputController->isForward()) m_camera->PitchDegree(-1);
		else if (m_inputController->isBack()) m_camera->PitchDegree(1);

		// TODO: 将此替换为应用程序内容的更新函数。
		m_sceneRenderer->Update(m_timer);
		m_fpsTextRenderer->Update(m_timer);
		//m_carRenderer->Update(m_timer);
		m_skyRenderer->Update(m_timer);
	});
}

// 根据当前应用程序状态呈现当前帧。
// 如果帧已呈现并且已准备好显示，则返回 true。
bool JobMain::Render() 
{
	// 在首次更新前，请勿尝试呈现任何内容。
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// 将视区重置为针对整个屏幕。
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// 将呈现目标重置为屏幕。
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// 清除后台缓冲区和深度模具视图。
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 呈现场景对象。
	// TODO: 将此替换为应用程序内容的渲染函数。
	m_sceneRenderer->Render();
	m_fpsTextRenderer->Render();
	//m_carRenderer->Render();
	m_skyRenderer->Render();

	return true;
}

// 通知呈现器，需要释放设备资源。
void JobMain::OnDeviceLost()
{
	m_sceneRenderer->ReleaseDeviceDependentResources();
	m_fpsTextRenderer->ReleaseDeviceDependentResources();
	//m_carRenderer->ReleaseDeviceDependentResources();
	m_skyRenderer->ReleaseDeviceDependentResources();
}

// 通知呈现器，现在可重新创建设备资源。
void JobMain::OnDeviceRestored()
{
	m_sceneRenderer->CreateDeviceDependentResources();
	m_fpsTextRenderer->CreateDeviceDependentResources();
	//m_carRenderer->CreateDeviceDependentResources();
	m_skyRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
