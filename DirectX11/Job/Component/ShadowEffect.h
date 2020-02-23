#pragma once
#include "Utils/RenderToTexture.h"
#include "Apps/D3DApp.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Job {
	class ShadowEffect
	{
	public:
		ShadowEffect(const std::shared_ptr<D3DApp>& deviceResources, UINT width, UINT height);
		bool Render(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, UINT indexCount);
		void BeginRender();
		void EndRender();

		void SetModelMatrix(XMMATRIX matrix);
		void SetViewProjMatrix(XMFLOAT3 direction, XMFLOAT3 pos, float NearPlane, float FarPlane);

		ID3D11ShaderResourceView* GetShadowMapSRV() { return m_renderToTexture->GetShaderResourceView(); }
		XMMATRIX GetViewMatrix() { return XMLoadFloat4x4(&m_MVPData.view); }
		XMMATRIX GetProjMatrix() { return XMLoadFloat4x4(&m_MVPData.projection); }

	private:
		void Initialize();

		std::unique_ptr<RenderToTexture>		m_renderToTexture;
		std::shared_ptr<D3DApp>					m_deviceResource;
		ComPtr<ID3D11VertexShader>				m_vertexShader;
		ComPtr<ID3D11PixelShader>				m_pixelShader;
		ComPtr<ID3D11InputLayout>				m_inputLayout;
		ComPtr<ID3D11Buffer>					m_MVPConstantBuffer;
		ModelViewProjectionConstantBuffer		m_MVPData;

		UINT m_shadowMapWidth;
		UINT m_shadowMapHeight;

		XMFLOAT3	m_lightDirection;


		bool m_IsBegin;
	};
}
