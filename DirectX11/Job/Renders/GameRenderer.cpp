#include "pch.h"
#include "Utils/DirectXHelper.h"
#include "GameRenderer.h"

using namespace Job;
using namespace Windows::Foundation;
using namespace DXHelper;
using namespace DirectX;

GameRenderer::GameRenderer(const std::shared_ptr<D3DApp>& deviceResources,
	const std::shared_ptr<Camera>& camera, const std::shared_ptr<InputController>& input) :
	m_indexCount(0),
	m_deviceResources(deviceResources),
	m_camera(camera),
	m_input(input)
{
	m_mapModel = Model(m_deviceResources->GetD3DDevice());
	m_carModel = Model(m_deviceResources->GetD3DDevice());

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

	XMFLOAT3 eyePos = m_camera->getPosition();
	m_lightConstantBufferData.eyePos = XMFLOAT4(eyePos.x, eyePos.y, eyePos.z, 1.0f);

	if (m_input->GetKeyState(InputController::W))
	{
		for (int i = 1; i < 5; i++)
			m_carModel.SetModelMatrix(i, XMMatrixRotationX(XM_PI / 17));
	}
	if (m_input->GetKeyState(InputController::S))
	{
		for (int i = 1; i < 5; i++)
			m_carModel.SetModelMatrix(i, XMMatrixRotationX(XM_PI / 17));
	}
	if (m_input->IsKeyPressed(InputController::A)) 
	{
		m_carModel.SetModelMatrix(1, XMMatrixRotationY(XM_PI / 4));
		m_carModel.SetModelMatrix(2, XMMatrixRotationY(XM_PI / 4));
	}
	if (m_input->IsKeyPressed(InputController::D))
	{
		m_carModel.SetModelMatrix(1, XMMatrixRotationY(-XM_PI / 4));
		m_carModel.SetModelMatrix(2, XMMatrixRotationY(-XM_PI / 4));
	}
	if (m_input->IsKeyReleased(InputController::A))
	{
		m_carModel.SetModelMatrix(1, XMMatrixRotationY(-XM_PI / 4));
		m_carModel.SetModelMatrix(2, XMMatrixRotationY(-XM_PI / 4));
	}
	if (m_input->IsKeyReleased(InputController::D))
	{
		m_carModel.SetModelMatrix(1, XMMatrixRotationY(XM_PI / 4));
		m_carModel.SetModelMatrix(2, XMMatrixRotationY(XM_PI / 4));
	}

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
	//context->VSSetConstantBuffers1(0, 1, m_MVPConstantBuffer.GetAddressOf(), nullptr, nullptr);

	for (int i = 0; i < m_carModel.GetObjModels().size(); i++)
	{
		auto objdata = m_carModel.GetObjModels()[i];
		// 设置顶点缓冲
		context->IASetVertexBuffers(0, 1, objdata.vertexBuffer.GetAddressOf(), &stride, &offset);
		// 设置索引缓冲
		context->IASetIndexBuffer(objdata.indexBuffer.Get(), objdata.indexFormat, 0);
		// 设置纹理
		context->PSSetShaderResources(0, 1, objdata.texSRV.GetAddressOf());
		// 设置VS常量缓冲区
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(m_carModel.GetModelMatrix(i)));
		context->UpdateSubresource1(m_MVPConstantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
		context->VSSetConstantBuffers1(0, 1, m_MVPConstantBuffer.GetAddressOf(), nullptr, nullptr);
		// 设置PS常量缓冲区
		m_lightConstantBufferData.material = objdata.material;
		context->UpdateSubresource1(m_LightConstantBuffer.Get(), 0, NULL, &m_lightConstantBufferData, 0, 0, 0);
		context->PSSetConstantBuffers1(0, 1, m_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);
		// 绘图
		context->DrawIndexed(objdata.indexCount, 0, 0);
	}

	// 设置VS常量缓冲区
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(1.f, 1.f, 1.f)));
	context->UpdateSubresource1(m_MVPConstantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	context->VSSetConstantBuffers1(0, 1, m_MVPConstantBuffer.GetAddressOf(), nullptr, nullptr);
	for (int i = 0; i < m_mapModel.GetObjModels().size(); i++)
	{
		auto objdata = m_mapModel.GetObjModels()[i];
		// 设置顶点缓冲
		context->IASetVertexBuffers(0, 1, objdata.vertexBuffer.GetAddressOf(), &stride, &offset);
		// 设置索引缓冲
		context->IASetIndexBuffer(objdata.indexBuffer.Get(), objdata.indexFormat, 0);
		// 设置纹理
		context->PSSetShaderResources(0, 1, objdata.texSRV.GetAddressOf());
		// 设置PS常量缓冲区
		m_lightConstantBufferData.material = objdata.material;
		context->UpdateSubresource1(m_LightConstantBuffer.Get(), 0, NULL, &m_lightConstantBufferData, 0, 0, 0);
		context->PSSetConstantBuffers1(0, 1, m_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);
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
	CD3D11_BUFFER_DESC constantBufferDesc1(sizeof(LightConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc1, nullptr, &m_LightConstantBuffer)
	);

	// 创建车网格
	VertexPosNorTex carBody[] =
	{	// up
		{XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.f, 1.f, 0.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.f, 1.f, 0.f),XMFLOAT2(1.f, 0.f)},
		{XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.f, 1.f, 0.f),XMFLOAT2(0.f, 1.f)},
		{XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.f, 1.f, 0.f),XMFLOAT2(1.f, 1.f)},
		// bottom
		{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.f, -1.f, 0.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.f, -1.f, 0.f),XMFLOAT2(1.f, 0.f)},
		{XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.f, -1.f, 0.f),XMFLOAT2(0.f, 1.f)},
		{XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.f, -1.f, 0.f),XMFLOAT2(1.f, 1.f)},
		// forward
		{XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(0.f, 0.f, 1.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(0.f, 0.f, 1.f),XMFLOAT2(1.f, 0.f)},
		{XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.f, 0.f, 1.f),XMFLOAT2(0.f, 1.f)},
		{XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.f, 0.f, 1.f),XMFLOAT2(1.f, 1.f)},
		// back
		{XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(0.f, 0.f, -1.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(0.f, 0.f, -1.f),XMFLOAT2(1.f, 0.f)},
		{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0.f, 0.f, -1.f),XMFLOAT2(0.f, 1.f)},
		{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.f, 0.f, -1.f),XMFLOAT2(1.f, 1.f)},
		// left
		{XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(1.f, 0.f)},
		{XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.f, 1.f)},
		{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(1.f, 1.f)},
		// right
		{XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(1.f, 0.f)},
		{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.f, 1.f)},
		{XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(1.f, 1.f)},
		{XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(1.f, 1.f)}
	};

	unsigned short carBodyIndex[] =
	{
		0,1,3, 0,3,2, 4,5,7, 4,7,6, 8,9,11, 8,11,10,
		12,13,15, 12,15,14, 16,17,19, 16,19,18, 20,21,23, 20,23,22 
	};
	
	float cos30 = 0.866025f;
	float _cos30 = 0.133975f;
	VertexPosNorTex carWheel[] =
	{
		{XMFLOAT3(0.5f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.5f, 0.5f)},

		{XMFLOAT3(0.5f, 1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.5f, 0.0f)},
		{XMFLOAT3(0.5f, cos30, -0.5f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.75f, _cos30)},
		{XMFLOAT3(0.5f, 0.5f, -cos30), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(cos30, 0.5f)},
		{XMFLOAT3(0.5f, 0.f, -1.f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(1.f, 0.5f)},
		{XMFLOAT3(0.5f, -0.5f, -cos30), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(cos30, 0.75f)},
		{XMFLOAT3(0.5f, -cos30, -0.5f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.75f, cos30)},
		{XMFLOAT3(0.5f, -1.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.5f, 1.f)},
		{XMFLOAT3(0.5f, -cos30, 0.5f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.25f, cos30)},
		{XMFLOAT3(0.5f, -0.5f, cos30), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(_cos30, 0.75f)},
		{XMFLOAT3(0.5f, 0.f, 1.f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.f, 0.5f)},
		{XMFLOAT3(0.5f, 0.5f, cos30), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(_cos30, 0.5f)},
		{XMFLOAT3(0.5f, cos30, 0.5f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.25f, _cos30)},

		{XMFLOAT3(-0.5f, 0.f, 0.f), XMFLOAT3(1.f, 0.f, 0.f),XMFLOAT2(0.5f, 0.5f)},

		{XMFLOAT3(-0.5f, 1.f, 0.f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.5f, 0.0f)},
		{XMFLOAT3(-0.5f, cos30, 0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.75f, _cos30)},
		{XMFLOAT3(-0.5f, 0.5f, cos30), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(cos30, 0.5f)},
		{XMFLOAT3(-0.5f, 0.f, 1.f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(1.f, 0.5f)},
		{XMFLOAT3(-0.5f, -0.5f, cos30), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(cos30, 0.75f)},
		{XMFLOAT3(-0.5f, -cos30, 0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.75f, cos30)},
		{XMFLOAT3(-0.5f, -1.f, 0.f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.5f, 1.f)},
		{XMFLOAT3(-0.5f, -cos30, -0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.25f, cos30)},
		{XMFLOAT3(-0.5f, -0.5f, -cos30), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(_cos30, 0.75f)},
		{XMFLOAT3(-0.5f, 0.f, -1.f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.f, 0.5f)},
		{XMFLOAT3(-0.5f, 0.5f, -cos30), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(_cos30, 0.5f)},
		{XMFLOAT3(-0.5f, cos30, -0.5f), XMFLOAT3(-1.f, 0.f, 0.f),XMFLOAT2(0.25f, _cos30)},

		{XMFLOAT3(0.5f, 1.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f),XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(0.5f, cos30, -0.5f), XMFLOAT3(0.f, cos30, -0.5f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, 0.5f, -cos30), XMFLOAT3(0.f, 0.5f, -cos30),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, 0.f, -1.f), XMFLOAT3(0.f, 1.f, -1.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, -0.5f, -cos30), XMFLOAT3(0.f, -0.5f, -cos30),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, -cos30, -0.5f), XMFLOAT3(0.f, -cos30, -0.5f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, -1.f, 0.f), XMFLOAT3(0.f, -1.f, 0.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, -cos30, 0.5f), XMFLOAT3(0.f, cos30, 0.5f),XMFLOAT2(0.f, 0.)},
		{XMFLOAT3(0.5f, -0.5f, cos30), XMFLOAT3(0.f, -0.5f, cos30),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, 0.f, 1.f), XMFLOAT3(0.f, 1.f, 1.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, 0.5f, cos30), XMFLOAT3(0.f, 0.5f, cos30),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(0.5f, cos30, 0.5f), XMFLOAT3(0.f, cos30, 0.5f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, 1.f, 0.f), XMFLOAT3(0.f, 1.f, 0.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, cos30, 0.5f), XMFLOAT3(0.f, cos30, 0.5f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, 0.5f, cos30), XMFLOAT3(0.f, 0.5f, cos30),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, 0.f, 1.f), XMFLOAT3(0.f, 1.f, 1.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, -0.5f, cos30), XMFLOAT3(0.f, -0.5f, cos30),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, -cos30, 0.5f), XMFLOAT3(0.f, -cos30, 0.5f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, -1.f, 0.f), XMFLOAT3(0.f, -1.f, 0.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, -cos30, -0.5f), XMFLOAT3(0.f, cos30, -0.5f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, -0.5f, -cos30), XMFLOAT3(0.f, -0.5f, -cos30),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, 0.f, -1.f), XMFLOAT3(0.f, 1.f, -1.f),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, 0.5f, -cos30), XMFLOAT3(0.f, 0.5f, -cos30),XMFLOAT2(0.f, 0.f)},
		{XMFLOAT3(-0.5f, cos30, -0.5f), XMFLOAT3(0.f, cos30, -0.5f),XMFLOAT2(0.f, 0.f)}
	};

	unsigned short carWheelIndex[] =
	{
		0,1,2, 0,2,3, 0,3,4, 0,4,5, 0,5,6, 0,6,7, 0,7,8, 0,8,9, 0,9,10, 0,10,11, 0,11,12, 0,12,1,
		13,14,15, 13,15,16, 13,16,17, 13,17,18, 13,18,19, 13,19,20, 13,20,21, 13,21,22, 13,22,23, 13,23,24, 13,24,25, 13,25,14,

		26,38,49, 26,49,27, 27,49,48, 27,48,28, 28,48,47, 28,47,29, 29,47,46, 29,46,30, 30,46,45, 30,45,31,
		31,45,44, 31,44,32, 32,44,43, 32,43,33, 33,43,42, 33,42,34, 34,42,41, 34,41,35, 35,41,40, 35,40,36,
		36,40,39, 36,39,37, 37,39,38, 37,38,26
	};

	// 创建地图网格
	//if (!m_objReader.ReadObj(L"Assets/model2.obj"))
	//	return;
	//m_mapModel = Model(m_deviceResources->GetD3DDevice(), &m_objReader);

	Materials mat;
	mat.ambient = XMFLOAT4(0.694118f, 0.109804f, 0.584314f, 1.f);
	mat.diffuse = XMFLOAT4(0.694118f, 0.109804f, 0.584314f, 1.f);
	mat.specular = XMFLOAT4(0.350000f, 0.350000f, 0.350000f, 1.f);
	mat.reflect = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	m_carModel.AddObjPart(ARRAYSIZE(carBody), carBody, ARRAYSIZE(carBodyIndex), DXGI_FORMAT_R16_UINT,
		carBodyIndex, mat, L"Assets/mapTex/3Dxy_img_19.jpg");
	m_carModel.SetModelMatrix(0, XMMatrixMultiply(XMMatrixScaling(20.f, 10.f, 50.f), XMMatrixTranslation(0.f, 20.f, 0.f)));

	for (int i = 0; i < 4; i++)
		m_carModel.AddObjPart(ARRAYSIZE(carWheel), carWheel, ARRAYSIZE(carWheelIndex), DXGI_FORMAT_R16_UINT,
			carWheelIndex, mat, L"Assets/mapTex/3Dxy_img_15.jpg");
	m_carModel.SetModelMatrix(1, XMMatrixMultiply(XMMatrixScaling(5.f, 5.f, 5.f), XMMatrixTranslation(10.f, 15.f, 15.f)));
	m_carModel.SetModelMatrix(2, XMMatrixMultiply(XMMatrixScaling(5.f, 5.f, 5.f), XMMatrixTranslation(-10.f, 15.f, 15.f)));
	m_carModel.SetModelMatrix(3, XMMatrixMultiply(XMMatrixScaling(5.f, 5.f, 5.f), XMMatrixTranslation(10.f, 15.f, -15.f)));
	m_carModel.SetModelMatrix(4, XMMatrixMultiply(XMMatrixScaling(5.f, 5.f, 5.f), XMMatrixTranslation(-10.f, 15.f, -15.f)));

	// 修改加载成功变量
	//createCubeTask.then([this]() {
	m_loadingComplete = true;
	//});
}
