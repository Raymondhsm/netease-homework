#include "pch.h"
#include "Utils/DirectXHelper.h"
#include "Utils/ShaderStructures.h"
#include "ShadowEffect.h"

using namespace Job;

ShadowEffect::ShadowEffect(const std::shared_ptr<D3DApp>& deviceResources, UINT width, UINT height):
	m_deviceResource(deviceResources),
	m_shadowMapWidth(width),
	m_shadowMapHeight(height),
	m_IsBegin(false)
{
	m_renderToTexture = std::unique_ptr<RenderToTexture>(new RenderToTexture());
	m_renderToTexture->Initialize(m_deviceResource->GetD3DDevice(), m_shadowMapWidth, m_shadowMapHeight);

	// 设置三个矩阵
	SetModelMatrix(XMMatrixScaling(1.f, 1.f, 1.f));
	SetViewProjMatrix(XMFLOAT3(0.f, 0.f, 1.f), XMFLOAT3(0.f, 0.f, -2500.f), 1.f, 5000.f);

	Initialize();
}

void ShadowEffect::Initialize()
{
	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob;

	// 加载顶点和像素着色器
	ThrowIfFailed(
		DXHelper::CreateShaderFromFile(
			L"HSLS/DepthVS.cso",
			L"HLSL/DepthVS.hlsl",
			"VS", "vs_5_0", VSBlob.ReleaseAndGetAddressOf())
	);
	ThrowIfFailed(
		DXHelper::CreateShaderFromFile(
			L"HSLS/DepthPS.cso",
			L"HLSL/DepthPS.hlsl",
			"PS", "ps_5_0", PSBlob.ReleaseAndGetAddressOf())
	);

	// 加载着色器后 创建着色器
	ThrowIfFailed(
		m_deviceResource->GetD3DDevice()->CreateVertexShader(
			VSBlob->GetBufferPointer(),
			VSBlob->GetBufferSize(),
			nullptr,
			&m_vertexShader
		)
	);

	// 创建顶点描述
	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ThrowIfFailed(
		m_deviceResource->GetD3DDevice()->CreateInputLayout(
			vertexDesc,
			ARRAYSIZE(vertexDesc),
			VSBlob->GetBufferPointer(),
			VSBlob->GetBufferSize(),
			&m_inputLayout
		)
	);


	// 创建像素着色器
	ThrowIfFailed(
		m_deviceResource->GetD3DDevice()->CreatePixelShader(
			PSBlob->GetBufferPointer(),
			PSBlob->GetBufferSize(),
			nullptr,
			&m_pixelShader
		)
	);

	// 创建常量缓冲区0
	CD3D11_BUFFER_DESC constantBufferDesc0(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	ThrowIfFailed(
		m_deviceResource->GetD3DDevice()->CreateBuffer(&constantBufferDesc0, nullptr, &m_MVPConstantBuffer)
	);
}

bool ShadowEffect::Render(ID3D11Buffer * vertexBuffer, ID3D11Buffer * indexBuffer, UINT indexCount)
{
	if (!m_IsBegin) 
		return false;

	auto context = m_deviceResource->GetD3DDeviceContext();

	// 设置顶点数据
	UINT stride = sizeof(VertexPosNorTex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	context->UpdateSubresource1(m_MVPConstantBuffer.Get(), 0, NULL, &m_MVPData, 0, 0, 0);
	context->VSSetConstantBuffers1(0, 1, m_MVPConstantBuffer.GetAddressOf(), nullptr, nullptr);

	context->DrawIndexed(indexCount, 0, 0);

	return true;
}

void ShadowEffect::BeginRender()
{
	m_renderToTexture->SetRenderTarget(m_deviceResource->GetD3DDeviceContext());
	m_renderToTexture->ClearRenderTarget(m_deviceResource->GetD3DDeviceContext(), 0.f, 0.f, 0.f, 1.f);

	auto context = m_deviceResource->GetD3DDeviceContext();

	// 设置着色器
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());

	m_IsBegin = true;
}

void ShadowEffect::EndRender()
{
	m_deviceResource->ResetRenderTarget();

	m_IsBegin = false;
}

void ShadowEffect::SetModelMatrix(XMMATRIX matrix)
{
	XMStoreFloat4x4(&m_MVPData.model, XMMatrixTranspose(matrix));
}

void ShadowEffect::SetViewProjMatrix(XMFLOAT3 direction, XMFLOAT3 pos, float NearPlane, float FarPlane)
{
	XMVECTOR newPos = XMLoadFloat3(&pos);
	XMVECTOR lookDirection = XMLoadFloat3(&direction);
	XMVECTOR focusPos = newPos + lookDirection;
	XMVECTOR up = { 0.0f,1.0f,0.0f,0.0f };

	XMStoreFloat4x4(&m_MVPData.view, XMMatrixTranspose(XMMatrixLookAtRH(newPos, focusPos, up)));

	XMStoreFloat4x4(&m_MVPData.projection, XMMatrixTranspose(XMMatrixOrthographicRH((float)m_shadowMapWidth/2, (float)m_shadowMapHeight/2, NearPlane, FarPlane)));
}
