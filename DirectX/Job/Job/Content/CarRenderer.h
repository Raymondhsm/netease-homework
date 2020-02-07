#pragma once

#include "Common/DeviceResources.h"
#include "Common/StepTimer.h"
#include "ShaderStructures.h"

namespace Job 
{
class CarRenderer
{
	public:
		CarRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		//void StartTracking();
		//void TrackingUpdate(float positionX);
		//void StopTracking();
		//bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);

	private:
		// 缓存的设备资源指针。
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// 立体几何的 Direct3D 资源。
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// 立体几何的系统资源。
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// 用于渲染循环的变量。
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
};
}

