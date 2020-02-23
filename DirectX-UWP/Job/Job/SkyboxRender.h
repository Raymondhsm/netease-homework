#pragma once

#include "Common/DeviceResources.h"
#include "Common/StepTimer.h"
#include "Content/ShaderStructures.h"
#include "Common/Camera.h"

namespace Job
{
	class SkyboxRender
	{
	public:
		SkyboxRender(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<Camera> &camera);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:
		// 缓存的设备资源指针。
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::shared_ptr<Camera> m_camera;

		// 立体几何的 Direct3D 资源。
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// 纹理
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TexFrontSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TexBackSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TexLeftSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TexRightSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TexTopSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_TexBottomSRV;

		// 立体几何的系统资源。
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;
		bool m_loadingComplete;


	};
}

