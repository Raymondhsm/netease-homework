#pragma once

using namespace DirectX;

namespace Job {
	class Camera
	{
	public:
		Camera();
		Camera(const XMFLOAT3& pos, const XMFLOAT3& lookAt, const XMFLOAT3& worldUp);
		void Initialize();

		// 更新视锥体
		void UpdateFrustum(float fovAngleY, float aspectRatio, float nearZ, float farZ);
		void UpdateFrustum(float aspectRatio);

		// 更新视图矩阵
		void UpdateViewMatrix();

		// 设置相机位置
		void setPosition(float x, float y, float z);
		void setPosition(XMFLOAT3 pos);
		void setPosition(XMVECTOR pos);

		// 设置相机朝向（默认up垂直向上
		void setLookAt(float x, float y, float z);
		void setLookAt(XMFLOAT3 look);
		void setLookAt(XMVECTOR look);

		void setCameraV(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR worldUp);
		void setCamera(const XMFLOAT3& pos, const XMFLOAT3& lookAt, const XMFLOAT3& worldUp);

		// 获取位置
		XMFLOAT3 getPosition() { return m_position; }
		XMVECTOR getPositionV() { return XMLoadFloat3(&m_position); }

		// 获取朝向
		XMFLOAT3 getLookAt() { return m_lookAt; }
		XMVECTOR getLookAtV() { return XMLoadFloat3(&m_lookAt); }

		// 获取up方向
		XMFLOAT3 getUp() { return m_up; }
		XMVECTOR getUpV() { return XMLoadFloat3(&m_up); }

		// 获取right方向
		XMFLOAT3 getRight() { return m_right; }
		XMVECTOR getRightV() { return XMLoadFloat3(&m_right); }

		// 获取视图和投影矩阵
		XMMATRIX getView() const { return XMLoadFloat4x4(&m_view); }
		XMMATRIX getProjection() const { return XMLoadFloat4x4(&m_proj); }
		XMMATRIX getViewProjection() const { return XMLoadFloat4x4(&m_view) * XMLoadFloat4x4(&m_proj); }

		// 获取视锥体信息
		float getNearZ() const { return m_nearZ; }
		float getFarZ() const { return m_farZ; }
		float getFovY() const { return m_fovAngleY; }
		float getFovX() const { return atan(m_aspect * tan(m_fovAngleY * 0.5f)) * 2.f; }
		float getAspect() const { return m_aspect; }

		// 应用函数
		void Move(float distance, XMFLOAT3 direction);
		void Move(XMFLOAT3 dist);

		// 前后左右
		void Forward(float distance);
		void Back(float distance);
		void Left(float distance);
		void Right(float distance);

		// 俯仰角(x), 偏航角(y), 翻滚角(z)
		void Pitch(float angle);
		void Yaw(float angle);
		void Roll(float angle);

	private:
		// 定位向量信息
		XMFLOAT3 m_position;
		XMFLOAT3 m_lookAt;
		XMFLOAT3 m_up;
		XMFLOAT3 m_right;

		// 视图和投影矩阵
		XMFLOAT4X4 m_view;
		XMFLOAT4X4 m_proj;

		// 视锥体的信息
		float m_aspect;
		float m_fovAngleY;
		float m_nearZ;
		float m_farZ;
	};
}

