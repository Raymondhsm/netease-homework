#pragma once
#include "StepTimer.h"
#include <string>


class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp() {};

	int Run();
	void HandleDeviceLost();
	
	virtual bool Initialize();
	virtual void CreateWindowSizeDependentResource();
	virtual bool CreateDeviceDependentResource();
	virtual bool CreateMainWindow();      // 窗口初始化
	virtual void Update() = 0;
	virtual bool Render() = 0;
	virtual void Present() = 0;
	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceRestore() = 0;

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HINSTANCE	GetAppInst()const;                 // 获取应用实例的句柄
	HWND		GetMainWnd()const;                 // 获取主窗口句柄
	float		GetAspectRatio()const;             // 获取屏幕宽高比

protected:

	HINSTANCE	m_hAppInst;				// 应用实例句柄
	HWND		m_hMainWnd;				// 主窗口句柄
	bool		m_AppPaused;			// 应用是否暂停
	bool		m_Minimized;			// 应用是否最小化
	bool		m_Maximized;			// 应用是否最大化
	bool		m_Resizing;				// 窗口大小是否变化
	bool		m_Enable4xMsaa;			// 是否开启4倍多重采样
	UINT		m_4xMsaaQuality;		// MSAA支持的质量等级

	StepTimer m_Timer;           // 计时器

	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// Direct3D 11
	ComPtr<ID3D11Device3> m_pd3dDevice;                    // D3D11设备
	ComPtr<ID3D11DeviceContext3> m_pd3dContext;   // D3D11设备上下文
	ComPtr<IDXGISwapChain3> m_pSwapChain;                  // D3D11交换链

	// 缓存的设备属性。
	D3D_FEATURE_LEVEL	m_d3dFeatureLevel;
	
	// Direct3D 11.1
	//ComPtr<ID3D11Device1> m_pd3dDevice1;                  // D3D11.1设备
	//ComPtr<ID3D11DeviceContext1> m_pd3dImmediateContext1; // D3D11.1设备上下文
	//ComPtr<IDXGISwapChain1> m_pSwapChain1;                // D3D11.1交换链
	
	// 常用资源
	ComPtr<ID3D11Texture2D1> m_pDepthStencilBuffer;        // 深度模板缓冲区
	ComPtr<ID3D11RenderTargetView1> m_pRenderTargetView;   // 渲染目标视图
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;   // 深度模板视图
	D3D11_VIEWPORT m_ScreenViewport;                      // 视口

	// 派生类应该在构造函数设置好这些自定义的初始参数
	std::wstring m_MainWndCaption;                       // 主窗口标题
	int m_ClientWidth;                                   // 视口宽度
	int m_ClientHeight;                                  // 视口高度
};	

