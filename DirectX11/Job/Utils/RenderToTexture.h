#pragma once
#ifndef _RENDER_TO_TEXTURE_H_
#define _RENDER_TO_TEXTURE_H_

using namespace Microsoft::WRL;

class RenderToTexture
{
public:
	RenderToTexture();

	bool Initialize(ID3D11Device* d3dDevice, int TextureWidth, int TexureHeight);
	void ReleaseDeviceDependentResources();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha);
	
	ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView.Get(); };

private:
	ComPtr<ID3D11ShaderResourceView>		m_pShaderResourceView; 
	ComPtr<ID3D11Texture2D>					m_pDepthStencilBuffer;        
	ComPtr<ID3D11DepthStencilView>			m_pDepthStencilView;   
	D3D11_VIEWPORT							m_ScreenViewport;
};

#endif