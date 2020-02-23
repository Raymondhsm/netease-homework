#pragma once

using namespace DirectX;

namespace Job {
	class Camera 
	{
	public:
		Camera();
		Camera(XMFLOAT3 pos, XMFLOAT3 lookDirection, XMFLOAT3 up);
		
		// 设置视锥体(投影矩阵)
		void UpdateFrustum(float fovAngleY);
		void UpdateFrustum(float fovAngleY, float aspectRatio, float nearZ, float farZ);

		// 更新视图矩阵
		void UpdateViewMatrix();

		// 设置相机位置
		void setPosition(XMVECTOR pos);
		void setPosition(XMFLOAT3 pos);
		void setPosition(float x, float y, float z);

		// 设置相机朝向(方向，非定点)
		void setlookDirection(XMVECTOR lookDirection);
		void setlookDirection(XMFLOAT3 lookDirection);
		void setlookDirection(float x, float y, float z);

		// 设置相机
		void setCamera(XMVECTOR pos, XMVECTOR lookDirection, XMVECTOR worldUp);
		void setCamera(const XMFLOAT3& pos, const XMFLOAT3& lookDirection,const XMFLOAT3& worldUp);

		// 移动
		void Move(float distance, XMFLOAT3 direction);
		void Move(XMFLOAT3 dis);

		// 前后左右
		void Forward(float distance);
		void Back(float distance);
		void Left(float distance);
		void Right(float distance);

		// 旋转(X轴)
		void Pitch(float angle);
		void PitchDegree(float degree);

		// 旋转(Y轴)
		void Yaw(float angle);
		void YawDegree(float degree);

		// 绕定点（xz平面）旋转
		void RotateAtPoint(float angle);
		void RotateAtPointDegree(float degree);

		// 获取几个向量
		XMFLOAT3 getPosition()		{ return m_pos; }
		XMFLOAT3 getRight()			{ return m_right; }
		XMFLOAT3 getLook()			{ return m_look; }
		XMFLOAT3 getUp()			{ return m_up; }

		XMVECTOR getPosotionV()	const { return XMLoadFloat3(&m_pos); }
		XMVECTOR getRightV()		const { return XMLoadFloat3(&m_right); }
		XMVECTOR getLookV()		const { return XMLoadFloat3(&m_look); }
		XMVECTOR getUpV()			const { return XMLoadFloat3(&m_up); }

		// 获取视锥体信息
		float getNearZ()			const { return m_nearZ; }
		float getFarZ()				const { return m_farZ; }
		float getFovY()				const { return m_fovAngleY; }
		float getFovX()				const { return atan(m_aspectRatio * tan(m_fovAngleY * 0.5f)) * 2.f; }
		float getAspect()			const { return m_aspectRatio; }

		// 获取视图和投影矩阵
		XMMATRIX GetView()			const { return DirectX::XMLoadFloat4x4(&m_view); }
		XMMATRIX GetProj()			const { return DirectX::XMLoadFloat4x4(&m_proj); }
		XMMATRIX GetViewProj()		const { return XMLoadFloat4x4(&m_view) * XMLoadFloat4x4(&m_proj); }

	private:

		XMFLOAT3 m_pos;				// 位置
		XMFLOAT3 m_look;			// 朝向
		XMFLOAT3 m_right;			// 右方向
		XMFLOAT3 m_up;				// 上方向

		float m_fovAngleY;			// 视锥体Y方向张角
		float m_aspectRatio;		// 视锥体长宽比例
		float m_nearZ;				// 视锥体近平面
		float m_farZ;				// 视锥体远平面

		XMFLOAT4X4 m_view;			// 视图矩阵
		XMFLOAT4X4 m_proj;			// 投影矩阵
	};
}