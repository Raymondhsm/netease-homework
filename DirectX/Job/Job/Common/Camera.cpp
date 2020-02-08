#include "pch.h"
#include "Camera.h"

using namespace DirectX;
using namespace Job;

Camera::Camera():
	m_pos(0.f, 0.f, 0.f),
	m_look(0.f, 0.f, 1.f),
	m_up(0.f, 1.f, 0.f),
	m_right(1.f, 0.f, 0.f)
{
	this->UpdateFrustum(0.4f * XM_PI, 800.0f / 600, 1.0f, 1000.0f);
	this->UpdateViewMatrix();
}

Camera::Camera(XMFLOAT3 pos, XMFLOAT3 lookDirection, XMFLOAT3 up):
	m_pos(pos),
	m_look(lookDirection)
{
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR l = XMLoadFloat3(&lookDirection);
	XMVECTOR u = XMLoadFloat3(&up);

	XMVECTOR look = XMVector3Normalize(l);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(u, l));
	XMVECTOR newUp = XMVector3Normalize(XMVector3Cross(l, right));

	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, newUp);

	this->UpdateFrustum(0.4f * XM_PI, 800.0f / 600, 1.0f, 1000.0f);
	this->UpdateViewMatrix();
}

void Camera::UpdateFrustum(float fovAngleY)
{
	this->UpdateFrustum(fovAngleY, this->m_aspectRatio, this->m_nearZ, this->m_farZ);
}

void Camera::UpdateFrustum(float fovAngleY, float aspectRatio, float nearZ, float farZ)
{
	this->m_fovAngleY = fovAngleY;
	this->m_aspectRatio = aspectRatio;
	this->m_nearZ = nearZ;
	this->m_farZ = farZ;

	XMMATRIX P = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
	XMStoreFloat4x4(&m_proj, P);
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&m_pos);
	XMVECTOR lookDirection = XMLoadFloat3(&m_look);
	XMVECTOR focusPos = pos + lookDirection;
	XMVECTOR up = XMLoadFloat3(&m_up);

	XMStoreFloat4x4(&m_view, XMMatrixTranspose(XMMatrixLookAtRH(pos, focusPos, up)));
}

void Camera::setPosition(XMVECTOR pos)
{
	XMStoreFloat3(&m_pos, pos);
	this->UpdateViewMatrix();
}

void Camera::setPosition(XMFLOAT3 pos)
{
	XMVECTOR posV = XMLoadFloat3(&pos);
	this->setPosition(posV);
}

void Camera::setPosition(float x, float y, float z)
{
	XMVECTOR pos = XMVectorSet(x, y, z, 0.0f);
	this->setPosition(pos);
}

void Camera::setlookDirection(XMVECTOR lookDirection)
{
	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR look = XMVector3Normalize(lookDirection);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, lookDirection));
	XMVECTOR newUp = XMVector3Cross(lookDirection, right);

	XMStoreFloat3(&m_look, look);
	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, newUp);

	this->UpdateViewMatrix();
}

void Camera::setlookDirection(XMFLOAT3 lookDirection)
{
	this->setlookDirection(XMLoadFloat3(&lookDirection));
}

void Camera::setlookDirection(float x, float y, float z)
{
	this->setlookDirection(XMVectorSet(x, y, z, 0.0f));
}

void Camera::setCamera(XMVECTOR pos, XMVECTOR lookDirection, XMVECTOR worldUp)
{
	XMVECTOR look = XMVector3Normalize(lookDirection);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, look));
	XMVECTOR up = XMVector3Cross(look, right);

	XMStoreFloat3(&m_pos, pos);
	XMStoreFloat3(&m_look, look);
	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, up);
}

void Camera::setCamera(const XMFLOAT3 & pos, const XMFLOAT3 & lookDirection, const XMFLOAT3 & worldUp)
{
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR l = XMLoadFloat3(&lookDirection);
	XMVECTOR u = XMLoadFloat3(&worldUp);

	this->setCamera(p, l, u);
}

void Camera::Move(float distance, XMFLOAT3 direction)
{
	XMVECTOR newPos = XMLoadFloat3(&m_pos) + distance * XMLoadFloat3(&direction);
	this->setPosition(newPos);
}

void Camera::Move(XMFLOAT3 dis)
{
	XMVECTOR pos = XMLoadFloat3(&m_pos);
	XMVECTOR newPos = pos + XMLoadFloat3(&dis);

	this->setPosition(newPos);
}

void Camera::Forward(float distance)
{
	XMVECTOR direction = XMLoadFloat3(&m_look);
	XMFLOAT3 dis;
	XMStoreFloat3(&dis, distance * direction);
	this->Move(dis);
}

void Camera::Back(float distance)
{
	XMVECTOR direction = -XMLoadFloat3(&m_look);
	XMFLOAT3 dis;
	XMStoreFloat3(&dis, distance * direction);
	this->Move(dis);
}

void Camera::Left(float distance)
{
	XMVECTOR direction = -XMLoadFloat3(&m_right);
	XMFLOAT3 dis;
	XMStoreFloat3(&dis, distance * direction);
	this->Move(dis);
}

void Camera::Right(float distance)
{
	XMVECTOR direction = XMLoadFloat3(&m_right);
	XMFLOAT3 dis;
	XMStoreFloat3(&dis, distance * direction);
	this->Move(dis);
}

void Camera::Pitch(float angle)
{
	XMMATRIX ratationX = XMMatrixRotationX(angle);
	XMVECTOR oldLook = XMLoadFloat3(&m_look);
	XMVECTOR newLook = XMVector3Transform(oldLook, ratationX);

	this->setlookDirection(newLook);
}

void Camera::PitchDegree(float degree)
{
	float angle = degree * XM_PI / 180.0f;
	this->Pitch(angle);
}

void Camera::Yaw(float angle)
{
	XMMATRIX ratationY = XMMatrixRotationY(angle);
	XMVECTOR oldLook = XMLoadFloat3(&m_look);
	XMVECTOR newLook = XMVector3Transform(oldLook, ratationY);

	this->setlookDirection(newLook);
}

void Camera::YawDegree(float degree)
{
	float angle = degree * XM_PI / 180.0f;
	this->Yaw(angle);
}

void Camera::RotateAtPoint(float angle)
{

}

void Camera::RotateAtPointDegree(float degree)
{
}







