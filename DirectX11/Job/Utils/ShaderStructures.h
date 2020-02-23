#pragma once
#include "pch.h"
#include "Utils/ObjReader.h"
#include "Utils/DirectXHelper.h"
#include "Utils/DDSTextureLoader.h"
#include "Utils/WICTextureLoader.h"

using namespace DirectX;

namespace Job {
	// 用于向顶点着色器发送 MVP 矩阵的常量缓冲区。
	struct ModelViewProjectionConstantBuffer
	{
		XMFLOAT4X4 model;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct ModelViewProjNorConstantBuffer
	{
		XMFLOAT4X4 model;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4X4 worldInvTranspose;
	};

	struct ModelViewProjNorShadowConstantBuffer
	{
		XMFLOAT4X4 model;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4X4 worldInvTranspose;
		XMFLOAT4X4 shadowView;
		XMFLOAT4X4 shadowProj;
	};

	struct Materials
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT4 reflect;

		Materials& operator=(const Materials& m) = default;
		Materials& operator=(const Material& m)
		{
			this->ambient = m.ambient;
			this->diffuse = m.diffuse;
			this->specular = m.specular;
			this->reflect = m.reflect;
			return *this;
		}
	};

	struct DirectionLight
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT3 direction;
		float waste;         // 填充位
	};

	struct LightConstantBuffer
	{
		Materials material;
		DirectionLight light;
		XMFLOAT3 eyePos;
		float waste;  // 填充位
	};

	// 用于向顶点着色器发送每个顶点的数据。
	struct VertexPositionColor
	{
		XMFLOAT3 pos;
		XMFLOAT3 color;
	};

	struct VertexPosTex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};

	struct VertexPosNorTex
	{
		XMFLOAT3 pos;
		XMFLOAT3 nor;
		XMFLOAT2 tex;
	};

	struct ObjModel
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>				indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	texSRV;
		Materials											material;

		UINT vertexCount;
		UINT indexCount;
		DXGI_FORMAT indexFormat;
	};

}

