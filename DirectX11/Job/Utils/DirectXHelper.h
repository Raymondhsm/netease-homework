#pragma once
#ifndef DIRECTXHELPER_H_
#define DIRECTXHELPER_H_


#include <ppltasks.h>

namespace DXHelper
{
	HRESULT WINAPI DXTraceW(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr, _In_opt_ const WCHAR* strMsg, _In_ bool bPopMsgBox);

#if defined(DEBUG) | defined(_DEBUG)
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)												\
	{															\
		HRESULT hresult = (x);										\
		if(FAILED(hresult))											\
		{														\
			DXHelper::DXTraceW(__FILEW__, (DWORD)__LINE__, hresult, L#x, true);\
		}														\
	}
#endif
#else
#ifndef ThrowIfFailed
#define ThrowIfFailed(x) (x)
#endif 
#endif


#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }
	HRESULT CreateShaderFromFile(
		const WCHAR* csoFileNameInOut,
		const WCHAR* hlslFileName,
		LPCSTR entryPoint,
		LPCSTR shaderModel,
		ID3DBlob** ppBlobOut);
}


#endif