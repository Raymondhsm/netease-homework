#include "pch.h"
#include "Utils/DirectXHelper.h"
#include "Utils/WICTextureLoader.h"
#include "GameRenderer.h"

using namespace Job;
using namespace Windows::Foundation;
using namespace DXHelper;

GameRenderer::GameRenderer(const std::shared_ptr<D3DApp>& deviceResources, const std::shared_ptr<Camera>& camera) :
	m_indexCount(0),
	m_deviceResources(deviceResources),
	m_camera(camera)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void GameRenderer::CreateWindowSizeDependentResources()
{
	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(m_camera->GetProj()));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(m_camera->GetView()));
}

void GameRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_MVPConstantBuffer.Reset();
	m_LightConstantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}

void GameRenderer::Update(StepTimer const & timer)
{
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(m_camera->GetView()));
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(1.f, 1.f, 1.f)));
}

void GameRenderer::Render()
{
	// 确保加载完成后进行渲染操作
	if (!m_loadingComplete) return;

	auto context = m_deviceResources->GetD3DDeviceContext();

	// 设置着色器
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// 将缓冲数据发送到常量缓冲区
	context->UpdateSubresource1(m_MVPConstantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	// 设置顶点数据
	UINT stride = sizeof(VertexPosNorTex);
	UINT offset = 0;
	//context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// 设置index缓冲
	//context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//context->IASetInputLayout(m_inputLayout.Get());

	// 将常量缓冲区发送到图形设备。
	//context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

	// 绘制对象。
	//context->PSSetShaderResources(0, 1, m_TexFrontSRV.GetAddressOf());
	//context->DrawIndexed(6, 0, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetConstantBuffers1(0, 1, m_MVPConstantBuffer.GetAddressOf(), nullptr, nullptr);
	for (int i = 0; i < m_mapModel.size(); i++)
	{
		auto objdata = m_mapModel[i];
		// 设置顶点缓冲
		context->IASetVertexBuffers(0, 1, objdata.vertexBuffer.GetAddressOf(), &stride, &offset);
		// 设置索引缓冲
		context->IASetIndexBuffer(objdata.indexBuffer.Get(), objdata.indexFormat, 0);
		// 设置纹理
		context->PSSetShaderResources(0, 1, objdata.texSRV.GetAddressOf());
		// 设置PS常量缓冲区
		//context->UpdateSubresource1(m_LightConstantBuffer.Get(), 0, NULL, &objdata.material, 0, 0, 0);
		//context->PSSetConstantBuffers1(0, 1, m_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);
		// 绘图
		context->DrawIndexed(objdata.indexCount, 0, 0);
	}
}


void GameRenderer::CreateDeviceDependentResources()
{
	Microsoft::WRL::ComPtr<ID3DBlob> VSBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> PSBlob;

	// 加载顶点和像素着色器
	ThrowIfFailed(
		CreateShaderFromFile(
			L"HSLS/GameVS.cso",
			L"HLSL/GameVS.hlsl",
			"VS", "vs_5_0", VSBlob.ReleaseAndGetAddressOf())
	);
	ThrowIfFailed(
		CreateShaderFromFile(
			L"HSLS/GamePS.cso",
			L"HLSL/GamePS.hlsl",
			"PS", "ps_5_0", PSBlob.ReleaseAndGetAddressOf())
	);

	// 加载着色器后 创建着色器
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
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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


	// 创建像素着色器
	ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreatePixelShader(
			PSBlob->GetBufferPointer(),
			PSBlob->GetBufferSize(),
			nullptr,
			&m_pixelShader
		)
	);

	// 创建常量缓冲区0
	CD3D11_BUFFER_DESC constantBufferDesc0(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc0, nullptr, &m_MVPConstantBuffer)
	);

	// 创建常量缓冲区1
	/*CD3D11_BUFFER_DESC constantBufferDesc1(sizeof(LightConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc1, nullptr, &m_LightConstantBuffer)
	);*/


	// 创建网格
	if (!m_objReader.ReadObj(L"Assets/test.obj"))
		return;
	for (int i = 0; i < m_objReader.objParts.size(); i++)
	{
		m_mapModel.push_back(ObjModel(m_deviceResources->GetD3DDevice(), m_objReader.objParts[i]));
	}



	// 创建顶点缓冲数据
	//D3D11_SUBRESOURCE_DATA VBData = { 0 };
	//VBData.pSysMem = vertices;
	//VBData.SysMemPitch = 0;
	//VBData.SysMemSlicePitch = 0;
	//CD3D11_BUFFER_DESC VBDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
	/*ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(
			&VBDesc,
			&VBData,
			&m_vertexBuffer
		)
	);*/

	// 创建索引缓冲数据
	//static const unsigned short indices[] =
	//{
	//	1,0,2,//front
	//	3,1,2,

	//	6,4,5,//left
	//	6,5,7,

	//	8,10,11,//right
	//	8,11,9,

	//	12,14,15,//back
	//	12,15,13,

	//	16,18,19,//bottom
	//	16,19,17,

	//	20,22,23,//top
	//	20,23,21
	//};

	//m_indexCount = ARRAYSIZE(indices);

	//D3D11_SUBRESOURCE_DATA IBData = { 0 };
	//IBData.pSysMem = indices;
	//IBData.SysMemPitch = 0;
	//IBData.SysMemSlicePitch = 0;
	//CD3D11_BUFFER_DESC IBDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
	//ThrowIfFailed(
	//	m_deviceResources->GetD3DDevice()->CreateBuffer(
	//		&IBDesc,
	//		&IBData,
	//		&m_indexBuffer
	//	)
	//);
	//});


	//CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/front.dds", nullptr, m_TexFrontSRV.GetAddressOf());


	// 修改加载成功变量
	//createCubeTask.then([this]() {
	m_loadingComplete = true;
	//});
}
