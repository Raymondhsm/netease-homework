#include "pch.h"
#include "InputController.h"


InputController::InputController() 
{
	m_window = CoreWindow::GetForCurrentThread();
	m_firstPersonView = true;

	Initialize();
}

void InputController::Initialize() 
{
	m_window->PointerPressed += 
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &InputController::OnPointerPressed);
	m_window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &InputController::OnPointerMoved);
	m_window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &InputController::OnPointerReleased);
	m_window->PointerWheelChanged +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &InputController::OnPointerWheelChanged);
	m_window->KeyDown +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &InputController::OnKeyDown);
	m_window->KeyUp +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &InputController::OnKeyUp);
}

void InputController::Update()
{
	m_wheelChange = 0.0f;

	m_pointerLastPos = m_pointerPos;

	m_pointerPos = m_pointerPosDelta;
}

void InputController::OnPointerPressed(_In_ CoreWindow^ sender, _In_ PointerEventArgs^ arg)
{

}

void InputController::OnPointerMoved(_In_ CoreWindow^ sender, _In_ PointerEventArgs^ arg)
{
	auto pos = arg->CurrentPoint->Position;

	m_pointerPosDelta.x = pos.X;
	m_pointerPosDelta.y = pos.Y;
}

void InputController::OnPointerReleased(_In_ CoreWindow^ sender, _In_ PointerEventArgs^ arg)
{

}

void InputController::OnPointerWheelChanged(_In_ CoreWindow^ sender, _In_ PointerEventArgs^ arg)
{
	int offset = arg->CurrentPoint->Properties->MouseWheelDelta;
	m_wheelChange += offset;
}

void InputController::OnKeyDown(_In_ CoreWindow^ sender, _In_ KeyEventArgs^ arg)
{
	VirtualKey key = arg->VirtualKey;

	switch (key)
	{
	case VirtualKey::W:
		m_forward = true;
		break;

	case VirtualKey::S:
		m_back = true;
		break;

	case VirtualKey::A:
		m_left = true;
		break;

	case VirtualKey::D:
		m_right = true;
		break;

	case VirtualKey::Up:
		m_lengthenDistance = true;
		break;

	case VirtualKey::Down:
		m_shortenDistance = true;
		break;

	}
}

void InputController::OnKeyUp(_In_ CoreWindow^ sender, _In_ KeyEventArgs^ arg)
{
	VirtualKey key = arg->VirtualKey;

	switch (key)
	{
	case VirtualKey::W:
		m_forward = false;
		break;

	case VirtualKey::S:
		m_back = false;
		break;

	case VirtualKey::A:
		m_left = false;
		break;

	case VirtualKey::D:
		m_right = false;
		break;

	case VirtualKey::Up:
		m_lengthenDistance = false;
		break;

	case VirtualKey::Down:
		m_shortenDistance = false;
		break;

	case VirtualKey::V:
		m_firstPersonView = !m_firstPersonView;
		break;
	}
}