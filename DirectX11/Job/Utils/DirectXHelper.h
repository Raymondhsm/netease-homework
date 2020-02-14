#pragma once
#ifndef DIRECTXHELPER_H_
#define DIRECTXHELPER_H_


#include <ppltasks.h>

namespace DXHelper
{
	HRESULT WINAPI DXTraceW(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_ bool bPopMsgBox);

	inline void ThrowIfFailed(HRESULT hr, const wchar_t* message = L"Failed")
	{
		if (FAILED(hr))
		{
#if defined(DEBUG) | defined(_DEBUG)
			DXHelper::DXTraceW(__FILEW__, (DWORD)__LINE__, hr, true);
#else
			// 在此行中设置断点，以捕获 Win32 API 错误。
			MessageBox(0, message, 0, 0);
#endif
		}
	}

#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }
	HRESULT CreateShaderFromFile(
		const WCHAR* csoFileNameInOut,
		const WCHAR* hlslFileName,
		LPCSTR entryPoint,
		LPCSTR shaderModel,
		ID3DBlob** ppBlobOut);
}


#endif