#pragma once
#include "Apps/D3DApp.h"
#include "Renders/SkyboxRender.h"
#include "Component/Camera.h"

class GameController :public D3DApp
{
public:
	GameController(HINSTANCE hInstance);
	~GameController();

	void Update();
	bool Render();
	void Present();
	void OnDeviceLost();
	void OnDeviceRestore();
	void CreateWindowSizeDependentResource();

private:
	std::shared_ptr<Job::Camera> m_camera;

	std::unique_ptr<Job::SkyboxRender> m_skyRenderer;
};

