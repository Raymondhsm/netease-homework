#pragma once
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Devices::Input;
using namespace DirectX;
#include <DirectXMath.h>

ref class InputController
{
public:
	// 接收鼠标和键盘输入的响应函数
	void OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ arg);
	void OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ arg);
	void OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ arg);
	void OnPointerWheelChanged(CoreWindow^ sender, PointerEventArgs^ arg);
	void OnKeyDown(CoreWindow^ sender, KeyEventArgs^ arg);
	void OnKeyUp(CoreWindow^ sender, KeyEventArgs^ arg);

internal:
	InputController();
	void Update();

	// 外部接口
	bool isForward() { return m_forward; }
	bool isBack() { return m_back; }
	bool isLeft() { return m_left; }
	bool isRight() { return m_right; }

	bool isFirstPersonView() { return m_firstPersonView; }
	bool isShortenDistance() { return m_shortenDistance; }
	bool isLengthenDistance() { return m_lengthenDistance; }

	float getPointerHorizon() { return m_pointerHorizon; }
	float getPointerVertical() { return m_pointerVertical; }
	float getWheelChange() { return m_wheelChange; }

	XMFLOAT2 getPointerPos() { return m_pointerPos; }
	XMFLOAT2 getPointLastPos() { return m_pointerLastPos; }
	XMFLOAT2 getPointerMoved() { return XMFLOAT2(m_pointerPos.x - m_pointerLastPos.x, m_pointerPos.y - m_pointerLastPos.y); }

private:

	CoreWindow^ m_window;

	// 用于行走判断的变量
	bool m_forward;
	bool m_back;
	bool m_left;
	bool m_right;

	// 用于其他操作判断变量
	bool m_firstPersonView;
	bool m_shortenDistance;
	bool m_lengthenDistance;

	// 鼠标移动变化变量
	float m_pointerHorizon;
	float m_pointerVertical;
	float m_wheelChange;

	// 用于储存鼠标位置
	XMFLOAT2 m_pointerPos;
	XMFLOAT2 m_pointerLastPos;
	XMFLOAT2 m_pointerPosDelta;

	void Initialize();
};

