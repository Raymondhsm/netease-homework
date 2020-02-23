#include "pch.h"
#include <DirectXMath.h>
#include "Camera.h"

using namespace Job;

Camera::Camera()
{
	Initialize();
	UpdateFrustum(0.4f*XM_PI, 800.f / 600, 1.f, 1000.f);
	UpdateViewMatrix();
}

Camera::Camera(const XMFLOAT3 & pos, const XMFLOAT3 & lookAt, const XMFLOAT3 & worldUp)
{
	setCamera(pos, lookAt, worldUp);
	UpdateFrustum(0.4f*XM_PI, 800.f / 600, 1.f, 1000.f);
}

void Camera::Initialize()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_lookAt = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_right = XMFLOAT3(1.0f, 0.0f, 0.0f);
}

void Camera::UpdateFrustum(float fovAngleY, float aspectRatio, float nearZ, float farZ)
{
	this->m_fovAngleY = fovAngleY;
	this->m_aspect = aspectRatio;
	this->m_nearZ = nearZ;
	this->m_farZ = farZ;

	this->UpdateFrustum(aspectRatio);
}

void Camera::UpdateFrustum(float aspectRatio)
{
	this->m_aspect = aspectRatio;

	// º∆À„Õ∂”∞æÿ’Û
	XMMATRIX proj = XMMatrixPerspectiveFovLH(this->m_fovAngleY, aspectRatio, this->m_nearZ, this->m_farZ);
	XMStoreFloat4x4(&m_proj, proj);
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR eye = XMLoadFloat3(&this->m_position);
	XMVECTOR lookAt = XMLoadFloat3(&this->m_lookAt);
	XMVECTOR up = XMLoadFloat3(&this->m_up);

	XMStoreFloat4x4(&this->m_view, XMMatrixTranspose(XMMatrixLookAtRH(eye, lookAt, up)));
}

void Camera::setPosition(float x, float y, float z)
{
	this->setPosition(XMFLOAT3(x, y, z));
}

void Camera::setPosition(XMFLOAT3 pos)
{
	this->m_position = pos;
	this->UpdateViewMatrix();
}

void Camera::setPosition(XMVECTOR pos)
{
	XMStoreFloat3(&this->m_position, pos);
	this->UpdateViewMatrix();
}

void Camera::setLookAt(float x, float y, float z)
{
	this->setLookAt(XMVectorSet(x, y, z, 0.0f));
}

void Camera::setLookAt(XMFLOAT3 lookAt)
{
	this->setLookAt(XMLoadFloat3(&lookAt));
}

void Camera::setLookAt(XMVECTOR lookAt)
{
	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR pos = XMLoadFloat3(&this->m_position);
	this->setCameraV(pos, lookAt, worldUp);
}

void Camera::setCameraV(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR worldUp)
{
	XMVECTOR look = XMVector3Normalize(lookAt - pos);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, look));
	XMVECTOR up = XMVector3Cross(look, right);

	XMStoreFloat3(&m_position, pos);
	XMStoreFloat3(&m_lookAt, look);
	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, up);

	UpdateViewMatrix();
}

void Camera::setCamera(const XMFLOAT3 & pos, const XMFLOAT3 & lookAt, const XMFLOAT3 & worldUp)
{
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR l = XMLoadFloat3(&lookAt);
	XMVECTOR u = XMLoadFloat3(&worldUp);

	setCameraV(p, l, u);
}

void Camera::Move(float distance, XMFLOAT3 direction)
{
	XMVECTOR dire = XMVector3Normalize(XMLoadFloat3(&direction));
	XMVECTOR result = distance * dire;
	XMVECTOR oldPos = XMLoadFloat3(&this->m_position);
	XMVECTOR newPos = oldPos + result;
	this->setPosition(newPos);
}

void Camera::Move(XMFLOAT3 dist)
{
	XMVECTOR oldPos = XMLoadFloat3(&this->m_position);
	XMVECTOR result = XMLoadFloat3(&dist);
	XMVECTOR newPos = oldPos + result;
	this->setPosition(newPos);
}

void Camera::Forward(float distance)
{
	this->Move(distance, this->m_lookAt);
}

void Camera::Back(float distance)
{
	XMVECTOR backDire = -XMLoadFloat3(&this->m_lookAt);
	XMFLOAT3 back;
	XMStoreFloat3(&back, backDire);
	this->Move(distance, back);
}

void Camera::Left(float distance)
{
	XMVECTOR leftDire = -XMLoadFloat3(&this->m_right);
	XMFLOAT3 left;
	XMStoreFloat3(&left, leftDire);
	this->Move(distance, left);
}

void Camera::Right(float distance)
{
	this->Move(distance, this->m_right);
}

// »∆X÷·
void Camera::Pitch(float angle)
{
	XMMATRIX ratationX = XMMatrixRotationX(angle);
	XMVECTOR oldLook = XMLoadFloat3(&this->m_lookAt);
	XMVECTOR newLook = XMVector3Transform(oldLook, ratationX);
	this->setLookAt(newLook);
}

// »∆Y÷·
void Camera::Yaw(float angle)
{
	XMMATRIX ratationY = XMMatrixRotationY(angle);
	XMVECTOR oldLook = XMLoadFloat3(&this->m_lookAt);
	XMVECTOR newLook = XMVector3Transform(oldLook, ratationY);
	this->setLookAt(newLook);
}

// »∆Z÷·
void Camera::Roll(float angle)
{
	return;
}
