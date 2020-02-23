#include "pch.h"
#include "CarRenderer.h"
#include "Common/DirectXHelper.h"

using namespace DirectX;
using namespace Windows::Foundation;

namespace Job {
	CarRenderer::CarRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources):
		m_loadingComplete(false),
		m_degreesPerSecond(45),
		m_indexCount(0),
		m_tracking(false),
		m_deviceResources(deviceResources)
	{
		CreateDeviceDependentResources();
		CreateWindowSizeDependentResources();
	}

	void CarRenderer::CreateDeviceDependentResources()
	{
		// 加载顶点和像素着色器
		auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
		auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

		// 加载着色器后 创建着色器
		auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateVertexShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					&m_vertexShader
				)
			);

			// 创建顶点描述
			static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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


		// 创建像素着色器
		auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreatePixelShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					&m_pixelShader
				)
			);

			// 创建常量缓冲区
			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateBuffer(
					&constantBufferDesc,
					nullptr,
					&m_constantBuffer
				)
			);
		});

		// 创建网格
		auto createCubeTask = (createVSTask && createPSTask).then([this]() {
			// 创建网格位置和颜色
			static const VertexPositionColor vertices[] = {
				{XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
				{XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
				{XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
				{XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
				{XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
				{XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)}
			};

			// 创建顶点缓冲数据
			D3D11_SUBRESOURCE_DATA VBData = { 0 };
			VBData.pSysMem = vertices;
			VBData.SysMemPitch = 0;
			VBData.SysMemSlicePitch = 0;
			CD3D11_BUFFER_DESC VBDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateBuffer(
					&VBDesc,
					&VBData,
					&m_vertexBuffer
				)
			);

			// 创建索引缓冲数据
			static const unsigned short indices[] =
			{
				4,2,0,
				0,2,5,
				0,3,4,
				5,3,0,
				1,2,4,
				5,2,1,
				4,3,1,
				1,3,5,
			};

			m_indexCount = ARRAYSIZE(indices);

			D3D11_SUBRESOURCE_DATA IBData = { 0 };
			IBData.pSysMem = indices;
			IBData.SysMemPitch = 0;
			IBData.SysMemSlicePitch = 0;
			CD3D11_BUFFER_DESC IBDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateBuffer(
					&IBDesc,
					&IBData,
					&m_indexBuffer
				)
			);
		});

		// 修改加载成功变量
		createCubeTask.then([this]() {
			m_loadingComplete = true;
		});
	}

	void CarRenderer::CreateWindowSizeDependentResources()
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

		// 请注意，OrientationTransform3D 矩阵在此处是后乘的，
		// 以正确确定场景的方向，使之与显示方向匹配。
		// 对于交换链的目标位图进行的任何绘制调用
		// 交换链呈现目标。对于到其他目标的绘制调用，
		// 不应应用此转换。

		// 此示例使用行主序矩阵利用右手坐标系。
		XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
			fovAngleY,
			aspectRatio,
			0.01f,
			100.0f
		);

		XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

		XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

		XMStoreFloat4x4(
			&m_constantBufferData.projection,
			XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

		// 眼睛位于(0,0.7,1.5)，并沿着 Y 轴使用向上矢量查找点(0,-0.1,0)。
		static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
		static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
		static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

		XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
	}

	void CarRenderer::ReleaseDeviceDependentResources()
	{
		m_loadingComplete = false;
		m_vertexShader.Reset();
		m_inputLayout.Reset();
		m_pixelShader.Reset();
		m_constantBuffer.Reset();
		m_vertexBuffer.Reset();
		m_indexBuffer.Reset();
	}

	void CarRenderer::Update(DX::StepTimer const & timer)
	{
		// 将度转换成弧度，然后将秒转换为旋转角度
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}

	// 将 3D 立方体模型旋转一定数量的弧度。
	void CarRenderer::Rotate(float radians)
	{
		// 准备将更新的模型矩阵传递到着色器
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
	}

	void CarRenderer::Render()
	{
		// 确保加载完成后进行渲染操作
		if (!m_loadingComplete) return;

		auto context = m_deviceResources->GetD3DDeviceContext();

		// 将缓冲数据发送到常量缓冲区
		context->UpdateSubresource1(
			m_constantBuffer.Get(),		// dest 
			0,
			NULL,
			&m_constantBufferData,		// src
			0,							// SrcRowPitch
			0,							// SrcDepthPitch
			0
		);

		UINT stride = sizeof(VertexPositionColor);
		UINT offset = 0;
		context->IASetVertexBuffers(
			0,			// StartSlot
			1,			// NumOfBuffer
			m_vertexBuffer.GetAddressOf(),
			&stride,	// stride
			&offset		// offset
		);

		context->IASetIndexBuffer(
			m_indexBuffer.Get(),
			DXGI_FORMAT_R16_UINT,	// unsigned short 16bit
			0						// offset
		);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->IASetInputLayout(m_inputLayout.Get());

		context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

		// 将常量缓冲区发送到图形设备。
		context->VSSetConstantBuffers1(
			0,			// StartSlot
			1,			// NumOfBuffer
			m_constantBuffer.GetAddressOf(),
			nullptr,
			nullptr
		);

		context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

		// 绘制对象。
		context->DrawIndexed(
			m_indexCount,
			0,			// GPU从索引缓冲区读取的第一个索引的位置
			0			// 从顶点缓冲区读取顶点之前添加到每个索引的值
		);

	}
}
