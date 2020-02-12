#pragma once
#include "../pch.h"
#include <ppltasks.h>

namespace DXHelper
{
	inline void ThrowIfFailed(HRESULT hr, wchar_t* message = L"Failed")
	{
		if (FAILED(hr))
		{
			// 在此行中设置断点，以捕获 Win32 API 错误。
			MessageBox(0, message, 0, 0);
		}
	}
}