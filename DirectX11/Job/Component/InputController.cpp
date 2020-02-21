#include "pch.h"
#include "InputController.h"

InputController::InputController()
{
	LButton = MouseState::UP;
	RButton = MouseState::UP;
	deltaWheelValue = 0.0f;
	currWheelValue = 0.0f;
	deltaMousePos.x = 0;
	deltaMousePos.y = 0;
	currMousePos.x = 0;
	currMousePos.y = 0;

	currState = { 0 };
	pressState = { 0 };
}

void InputController::ProcessMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_LBUTTONDOWN:
		LButton = MouseState::DOWN;
		break;

	case WM_LBUTTONUP:
		LButton = MouseState::UP;
		break;

	case WM_RBUTTONDOWN:
		RButton = MouseState::DOWN;
		break;

	case WM_RBUTTONUP:
		RButton = MouseState::UP;
		break;

	case WM_MOUSEWHEEL:
		OnWheelMove(GET_WHEEL_DELTA_WPARAM(wParam));
		break;

	}
}

void InputController::ProcessKeyboardMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
 	bool down = false;

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		down = true;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		break;

	default:
		return;
	}

	int vk = static_cast<int>(wParam);
	switch (vk)
	{
	case VK_SHIFT:
		vk = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
		if (!down)
		{
			// Workaround to ensure left vs. right shift get cleared when both were pressed at same time
			KeyUp(VK_LSHIFT, currState);
			KeyUp(VK_RSHIFT, currState);
		}
		break;

	case VK_CONTROL:
		vk = (lParam & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
		break;

	case VK_MENU:
		vk = (lParam & 0x01000000) ? VK_RMENU : VK_LMENU;
		break;
	}

	if (down)
	{
		KeyDown(vk, currState);
	}
	else
	{
		KeyUp(vk, currState);
	}
}

void InputController::Update()
{
	deltaMousePos.x = 0;
	deltaMousePos.y = 0;

	deltaWheelValue = 0.0f;

	// 更新键盘
	auto currPtr = reinterpret_cast<const uint32_t*>(&currState);
	auto prevPtr = reinterpret_cast<const uint32_t*>(&preState);
	auto releasedPtr = reinterpret_cast<uint32_t*>(&releaseState);
	auto pressedPtr = reinterpret_cast<uint32_t*>(&pressState);
	for (size_t j = 0; j < (256 / 32); ++j)
	{
		*pressedPtr = *currPtr & ~(*prevPtr);
		*releasedPtr = ~(*currPtr) & *prevPtr;

		++currPtr;
		++prevPtr;
		++releasedPtr;
		++pressedPtr;
	}

	preState = currState;
}

void InputController::OnMouseMove(float x, float y)
{
	if (currMousePos.x == 0.0f && currMousePos.y == 0.0f)
	{
		currMousePos.x = (long)x;
		currMousePos.y = (long)y;
	}

	deltaMousePos.x += (long)x - currMousePos.x;
	deltaMousePos.y += (long)y - currMousePos.y;

	currMousePos.x = (long)x;
	currMousePos.y = (long)y;
}

void InputController::OnWheelMove(float value)
{
	deltaWheelValue += value;
}

void InputController::KeyDown(int key, KeyState& state)
{
	if (key < 0 || key > 0xfe)
		return;

	auto ptr = reinterpret_cast<uint32_t*>(&state);

	unsigned int bf = 1u << (key & 0x1f);
	ptr[(key >> 5)] |= bf;
}

void InputController::KeyUp(int key, KeyState& state)
{
	if (key < 0 || key > 0xfe)
		return;

	auto ptr = reinterpret_cast<uint32_t*>(&state);

	unsigned int bf = 1u << (key & 0x1f);
	ptr[(key >> 5)] &= ~bf;
}
