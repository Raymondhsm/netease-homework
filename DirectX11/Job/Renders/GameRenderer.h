#pragma once
#include "Apps/StepTimer.h"
#include "Utils/ShaderStructures.h"
#include "Component/Camera.h"
#include "Component/InputController.h"
#include "Utils/ObjReader.h"
#include "Apps/D3DApp.h"
#include "Component/Model.h"
#include <vector>

using namespace Microsoft::WRL;
using namespace std;

namespace Job
{
	

	class GameRenderer
	{
	public:
		GameRenderer(const std::shared_ptr<D3DApp>& deviceResources, const std::shared_ptr<Camera> &camera, const std::shared_ptr<InputController>& input);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(StepTimer const& timer);
		void Render();

	private:
		void UpdateCarMove(float deltaTime);
		void UpdateCameraPos();

		// 缓存的设备资源指针。
		std::shared_ptr<D3DApp> m_deviceResources;
		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<InputController> m_input;

		ObjReader m_objReader;

		// 立体几何的 Direct3D 资源。
		ComPtr<ID3D11InputLayout>	m_inputLayout;
		ComPtr<ID3D11Buffer>		m_vertexBuffer;
		ComPtr<ID3D11Buffer>		m_indexBuffer;
		ComPtr<ID3D11VertexShader>	m_vertexShader;
		ComPtr<ID3D11PixelShader>	m_pixelShader;
		ComPtr<ID3D11Buffer>		m_MVPConstantBuffer;
		ComPtr<ID3D11Buffer>		m_LightConstantBuffer;

		Model			m_carModel;
		Model			m_mapModel;

		// 立体几何的系统资源。
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		LightConstantBuffer					m_lightConstantBufferData;

		__int32	m_indexCount;
		bool m_loadingComplete;

		// car model 的控制变量
		XMVECTOR m_carWheelRight;
		XMVECTOR m_moveDirection;
		float m_speed;
		float m_maxSpeed;
		float m_minSpeed;
		bool m_carStop;

		// 是否第一人称
		bool m_FP;
		float m_TPDistance;
	};
}

