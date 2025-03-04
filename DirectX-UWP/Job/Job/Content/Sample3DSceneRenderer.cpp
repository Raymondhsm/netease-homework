﻿#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "Common/DDSTextureLoader.h"
#include "..\Common\DirectXHelper.h"
#include <fstream>

using namespace Job;

using namespace DirectX;
using namespace Windows::Foundation;

// 从文件中加载顶点和像素着色器，然后实例化立方体几何图形。
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	ic = ref new InputController();
	static const XMFLOAT3 eye = { 10.0f, 10.0f, 10.0f };
	static const XMFLOAT3 at = { -10.0f, -10.0f, -10.0f };
	static const XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
	cam = new Camera(eye, at, up);
	objReader = new ObjReader();

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();


}

// 当窗口的大小改变时初始化视图参数。
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// 这是一个简单的更改示例，当应用程序在纵向视图或对齐视图中时，可以进行此更改
	//。
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	cam->UpdateFrustum(fovAngleY, aspectRatio, 0.01f, 100.0f);

	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(cam->GetProj()));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(cam->GetView()));


	//objReader->ReadObj("C:/Users/Administrator/source/repos/Project10/Project10/Debug/map.obj");
	//auto a = objReader->objParts;

	//std::ifstream wi(L"C:/Users/Administrator/source/repos/Project10/Project10/Debug/map.obj");
	//if (wi.is_open()) return;
}

// 每个帧调用一次，旋转立方体，并计算模型和视图矩阵。
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (true) {
		// 将度转换成弧度，然后将秒转换为旋转角度
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);

		/*XMMATRIX old = cam->GetView();
		cam->YawDegree(1);
		XMMATRIX New = cam->GetView();
		XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(cam->GetView()));*/
	}
	

}

// 将 3D 立方体模型旋转一定数量的弧度。
void Sample3DSceneRenderer::Rotate(float radians)
{
	// 准备将更新的模型矩阵传递到着色器
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// 进行跟踪时，可跟踪指针相对于输出屏幕宽度的位置，从而让 3D 立方体围绕其 Y 轴旋转。
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// 使用顶点和像素着色器呈现一个帧。
void Sample3DSceneRenderer::Render()
{
	// 加载是异步的。仅在加载几何图形后才会绘制它。
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// 准备常量缓冲区以将其发送到图形设备。
	context->UpdateSubresource1(
		m_constantBuffer.Get(),
		0,
		NULL,
		&m_constantBufferData,
		0,
		0,
		0
	);

	// 每个顶点都是 VertexPosTex 结构的一个实例。
	UINT stride = sizeof(VertexPosTex);
	UINT offset = 0;
	

	// 附加我们的顶点着色器。
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
		);

	// 将常量缓冲区发送到图形设备。
	context->VSSetConstantBuffers1(
		0,
		1,
		m_constantBuffer.GetAddressOf(),
		nullptr,
		nullptr
		);

	// 附加我们的像素着色器。
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
		);

	context->PSSetShaderResources(0, 1, m_TexSRV.GetAddressOf());



	context->IASetVertexBuffers(0, 1, m_vertexBuffer1.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	context->DrawIndexed(m_indexCount, 0, 0);

	 
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// 通过异步方式加载着色器。
	auto loadVSTask = DX::ReadDataAsync(L"TextureVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"TexturePixelShader.cso");

	// 加载顶点着色器文件之后，创建着色器和输入布局。
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	// 加载像素着色器文件后，创建着色器和常量缓冲区。
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);
	});

	// 加载两个着色器后，创建网格。
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

		// 加载网格顶点。每个顶点都有一个位置和一个颜色。
		static const VertexPosTex cubeVertices[] =
		{
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT2(0.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT2(0.0f, 1.0f)},
			{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT2(1.0f, 0.0f)},
			{XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
			{XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT2(1.0f, 1.0f)},
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);


		// 加载网格索引。每三个索引表示
		// 要在屏幕上呈现的三角形。
		// 例如: 0,2,1 表示顶点的索引
		// 顶点缓冲区中的索引为 0、2 和 1 的顶点构成了
		// 此网格的第一个三角形。
		static const unsigned short cubeIndices [] =
		{
			8,10,9, // -x
			9,10,11,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
				)
			);

		//加载纹理
		DX::ThrowIfFailed(
			CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/back.dds", nullptr, m_TexSRV.GetAddressOf())
		);
	});

	// 加载立方体后，就可以呈现该对象了。
	createCubeTask.then([this] () {
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_vertexBuffer1.Reset();
	//m_vertexBufferA.Reset();
	m_indexBuffer.Reset();
}