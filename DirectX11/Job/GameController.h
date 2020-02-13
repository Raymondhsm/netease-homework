#pragma once
#include "Apps/D3DApp.h"

class GameController :public D3DApp
{
public:
	GameController(HINSTANCE hInstance);
	~GameController() {};

	void Update();
	bool Render();
	void Present();
	void OnDeviceLost();
	void OnDeviceRestore();
	void CreateWindowSizeDependentResource();
};

