#include "pch.h"
#include "Utils/DirectXHelper.h"
#include "Utils/DDSTextureLoader.h"
#include "SkyboxRender.h"

using namespace Job;
using namespace Windows::Foundation;
using namespace DXHelper;

SkyboxRender::SkyboxRender(const std::shared_ptr<D3DApp>& deviceResources, const std::shared_ptr<Camera>& camera):
	m_indexCount(0),
	m_deviceResources(deviceResources),
	m_camera(camera)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void SkyboxRender::CreateDeviceDependentResources()
{
	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob;

	// 加载顶点和像素着色器
	ThrowIfFailed(
		CreateShaderFromFile(
			L"HSLS/SkyboxVS.cso",
			L"HLSL\\SkyboxVS.hlsl",
			"VS", "vs_5_0", VSBlob.ReleaseAndGetAddressOf())
	);
	ThrowIfFailed(
		CreateShaderFromFile(
			L"HSLS/SkyboxPS.cso", 
			L"HLSL\\SkyboxPS.hlsl", 
			"PS", "ps_5_0", PSBlob.ReleaseAndGetAddressOf())
	);

	// 加载着色器后 创建着色器
	//auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
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
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				VSBlob->GetBufferPointer(),
				VSBlob->GetBufferSize(),
				&m_inputLayout
			)
		);
	//});


	// 创建像素着色器
	//auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				PSBlob->GetBufferPointer(),
				PSBlob->GetBufferSize(),
				nullptr,
				&m_pixelShader
			)
		);

		// 创建常量缓冲区
		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
			)
		);
	//});

	// 创建网格
	//auto createCubeTask = (createVSTask && createPSTask).then([this]() {
		// 创建网格位置和颜色
		static const VertexPosTex vertices[] = {
			{XMFLOAT3(3000.0f, 3000.0f, 3000.0f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(3000.0f, -3000.0f, 3000.0f), XMFLOAT2(0.0f, 1.0f)},
			{XMFLOAT3(-3000.0f, 3000.0f, 3000.0f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(-3000.0f, -3000.0f, 3000.0f), XMFLOAT2(1.0f, 1.0f)},

			{XMFLOAT3(3000.0f, 3000.0f, 3000.0f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(3000.0f, -3000.0f, 3000.0f), XMFLOAT2(1.0f, 1.0f)},
			{XMFLOAT3(3000.0f, 3000.0f, -3000.0f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(3000.0f, -3000.0f, -3000.0f), XMFLOAT2(0.0f, 1.0f)},

			{XMFLOAT3(-3000.0f, 3000.0f, 3000.0f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(-3000.0f, -3000.0f, 3000.0f), XMFLOAT2(0.0f, 1.0f)},
			{XMFLOAT3(-3000.0f, 3000.0f, -3000.0f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(-3000.0f, -3000.0f, -3000.0f), XMFLOAT2(1.0f, 1.0f)},

			{XMFLOAT3(-3000.0f, 3000.0f, -3000.0f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(-3000.0f, -3000.0f, -3000.0f), XMFLOAT2(0.0f, 1.0f)},
			{XMFLOAT3(3000.0f, 3000.0f, -3000.0f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(3000.0f, -3000.0f, -3000.0f), XMFLOAT2(1.0f, 1.0f)},

			{XMFLOAT3(3000.0f, -3000.0f, 3000.0f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(3000.0f, -3000.0f, -3000.0f), XMFLOAT2(0.0f, 1.0f)},
			{XMFLOAT3(-3000.0f, -3000.0f, 3000.0f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(-3000.0f, -3000.0f, -3000.0f), XMFLOAT2(1.0f, 1.0f)},

			{XMFLOAT3(3000.0f, 3000.0f, -3000.0f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(3000.0f, 3000.0f, 3000.0f), XMFLOAT2(0.0f, 1.0f)},
			{XMFLOAT3(-3000.0f, 3000.0f, -3000.0f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(-3000.0f, 3000.0f, 3000.0f), XMFLOAT2(1.0f, 1.0f)},
			{XMFLOAT3(-3000.0f, 3000.0f, -3000.0f), XMFLOAT2(1.0f, 0.0f)}
		};

		// 创建顶点缓冲数据
		D3D11_SUBRESOURCE_DATA VBData = { 0 };
		VBData.pSysMem = vertices;
		VBData.SysMemPitch = 0;
		VBData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC VBDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
		ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&VBDesc,
				&VBData,
				&m_vertexBuffer
			)
		);

		// 创建索引缓冲数据
		static const unsigned short indices[] =
		{
			1,0,2,//front
			3,1,2,

			6,4,5,//left
			6,5,7,

			8,10,11,//right
			8,11,9,

			12,14,15,//back
			12,15,13,

			16,18,19,//bottom
			16,19,17,

			20,22,23,//top
			20,23,21
		};

		m_indexCount = ARRAYSIZE(indices);

		D3D11_SUBRESOURCE_DATA IBData = { 0 };
		IBData.pSysMem = indices;
		IBData.SysMemPitch = 0;
		IBData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC IBDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
		ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&IBDesc,
				&IBData,
				&m_indexBuffer
			)
		);
	//});

	
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/front.dds", nullptr, m_TexFrontSRV.GetAddressOf());
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/back.dds", nullptr, m_TexBackSRV.GetAddressOf());
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/top.dds", nullptr, m_TexTopSRV.GetAddressOf());
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/bottom.dds", nullptr, m_TexBottomSRV.GetAddressOf());
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/left.dds", nullptr, m_TexLeftSRV.GetAddressOf());
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/right.dds", nullptr, m_TexRightSRV.GetAddressOf());
	

	// 修改加载成功变量
	//createCubeTask.then([this]() {
		m_loadingComplete = true;
	//});
}

void SkyboxRender::CreateWindowSizeDependentResources()
{
	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(m_camera->GetProj()));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(m_camera->GetView()));
}

void SkyboxRender::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}

void SkyboxRender::Update(StepTimer const & timer)
{
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(m_camera->GetView()));
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixTranslationFromVector(m_camera->getPosotionV())));
}

void SkyboxRender::Render()
{
	// 确保加载完成后进行渲染操作
	if (!m_loadingComplete) return;

	auto context = m_deviceResources->GetD3DDeviceContext();

	// 设置着色器
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// 将缓冲数据发送到常量缓冲区
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	// 设置顶点数据
	UINT stride = sizeof(VertexPosTex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// 设置index缓冲
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());

	// 将常量缓冲区发送到图形设备。
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

	// 绘制对象。
	context->PSSetShaderResources(0, 1, m_TexFrontSRV.GetAddressOf());
	context->DrawIndexed(6, 0, 0);

	context->PSSetShaderResources(0, 1, m_TexLeftSRV.GetAddressOf());
	context->DrawIndexed(6, 6, 0);

	context->PSSetShaderResources(0, 1, m_TexRightSRV.GetAddressOf());
	context->DrawIndexed(6, 12, 0);

	context->PSSetShaderResources(0, 1, m_TexBackSRV.GetAddressOf());
	context->DrawIndexed(6, 18, 0);

	context->PSSetShaderResources(0, 1, m_TexBottomSRV.GetAddressOf());
	context->DrawIndexed(6, 24, 0);

	context->PSSetShaderResources(0, 1, m_TexTopSRV.GetAddressOf());
	context->DrawIndexed(6, 30, 0);
}
