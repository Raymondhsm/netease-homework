#include "pch.h"
#include "RenderToTexture.h"
#include "Utils/DirectXHelper.h"

RenderToTexture::RenderToTexture():
	m_pShaderResourceView(nullptr),
	m_pDepthStencilBuffer(nullptr),
	m_pDepthStencilView(nullptr)
{

}

bool RenderToTexture::Initialize(ID3D11Device * d3dDevice, int TextureWidth, int TexureHeight)
{
	// 准备深度缓冲的纹理
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = TextureWidth;
	depthBufferDesc.Height = TexureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; 
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;  
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	ThrowIfFailed(
		d3dDevice->CreateTexture2D(&depthBufferDesc, NULL, m_pDepthStencilBuffer.GetAddressOf())
	);


	// 准备深度缓冲区
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	ThrowIfFailed(
		d3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), &depthStencilViewDesc, m_pDepthStencilView.GetAddressOf())
	);


	// 准备资源视图
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; 
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
	ThrowIfFailed(
		d3dDevice->CreateShaderResourceView(m_pDepthStencilBuffer.Get(), &shaderResourceViewDesc, m_pShaderResourceView.GetAddressOf())
	);

	// 准备视口
	m_ScreenViewport.Width = (float)TextureWidth;
	m_ScreenViewport.Height = (float)TexureHeight;
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;
	m_ScreenViewport.TopLeftX = 0.0f;
	m_ScreenViewport.TopLeftY = 0.0f;


	return true;
}

void RenderToTexture::ReleaseDeviceDependentResources()
{
	m_pDepthStencilBuffer.Reset();
	m_pDepthStencilView.Reset();
	m_pShaderResourceView.Reset();
}

void RenderToTexture::SetRenderTarget(ID3D11DeviceContext * deviceContext)
{
	ID3D11RenderTargetView* renderTarget[1] = { 0 };
	deviceContext->OMSetRenderTargets(1, renderTarget, m_pDepthStencilView.Get());
	deviceContext->RSSetViewports(1, &m_ScreenViewport);
}

void RenderToTexture::ClearRenderTarget(ID3D11DeviceContext * deviceContext, float red, float green, float blue, float alpha)
{
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	deviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
