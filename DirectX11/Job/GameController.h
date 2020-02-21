#pragma once
#include "Apps/D3DApp.h"
#include "Renders/SkyboxRender.h"
#include "Renders/GameRenderer.h"
#include "Component/Camera.h"
#include "Component/InputController.h"

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

	void OnInputEvent(UINT message, WPARAM wParam, LPARAM lParam);

private:
	std::shared_ptr<Job::Camera> m_camera;
	std::shared_ptr<InputController> m_inputController;

	std::unique_ptr<Job::SkyboxRender> m_skyRenderer;
	std::unique_ptr<Job::GameRenderer> m_gameRender;
};

