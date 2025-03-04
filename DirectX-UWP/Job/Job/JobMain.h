﻿#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleFpsTextRenderer.h"
#include "Content\CarRenderer.h"
#include "Common/Camera.h"
#include "Common/InputController.h"
#include "SkyboxRender.h"

// 在屏幕上呈现 Direct2D 和 3D 内容。
namespace Job
{
	class JobMain : public DX::IDeviceNotify
	{
	public:
		JobMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~JobMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// 缓存的设备资源指针。
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// TODO: 替换为你自己的内容呈现器。
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;
		std::unique_ptr<CarRenderer> m_carRenderer;
		std::unique_ptr<SkyboxRender> m_skyRenderer;

		std::shared_ptr<Camera> m_camera;
		InputController^ m_inputController;

		// 渲染循环计时器。
		DX::StepTimer m_timer;
	};
}