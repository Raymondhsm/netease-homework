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

	struct Materials
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT4 reflect;

		Materials& operator=(const Material& m)
		{
			this->ambient = m.ambient;
			this->diffuse = m.diffuse;
			this->specular = m.specular;
			this->reflect = m.reflect;
			return *this;
		}
	};

	struct LightConstantBuffer
	{
		Materials material;
		XMFLOAT3 eyePos;
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

		ObjModel(ID3D11Device* device, ObjReader::ObjPart &obj)
		{
			
			this->vertexCount = (UINT)obj.vertices.size();

			// 设置顶点缓冲区描述
			D3D11_BUFFER_DESC vbd;
			ZeroMemory(&vbd, sizeof(vbd));
			vbd.Usage = D3D11_USAGE_IMMUTABLE;
			vbd.ByteWidth = vertexCount * (UINT)sizeof(VertexPosNormalTex);
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = 0;

			// 新建顶点缓冲区
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = obj.vertices.data();
			ThrowIfFailed(
				device->CreateBuffer(&vbd, &InitData, this->vertexBuffer.ReleaseAndGetAddressOf())
			);

			// 设置索引缓冲区描述
			D3D11_BUFFER_DESC ibd;
			ZeroMemory(&ibd, sizeof(ibd));
			ibd.Usage = D3D11_USAGE_IMMUTABLE;
			ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;
			if (this->vertexCount > 65535)
			{
				this->indexCount = (UINT)obj.indices32.size();
				this->indexFormat = DXGI_FORMAT_R32_UINT;
				ibd.ByteWidth = this->indexCount * (UINT)sizeof(DWORD);
				InitData.pSysMem = obj.indices32.data();

			}
			else
			{
				this->indexCount = (UINT)obj.indices16.size();
				this->indexFormat = DXGI_FORMAT_R16_UINT;
				ibd.ByteWidth = this->indexCount * (UINT)sizeof(WORD);
				InitData.pSysMem = obj.indices16.data();
			}
			// 新建索引缓冲区
			ThrowIfFailed(
				device->CreateBuffer(&ibd, &InitData, this->indexBuffer.ReleaseAndGetAddressOf())
			);


			// 创建漫射光对应纹理
			auto& strD = obj.texStrDiffuse;
			if (strD.size() > 4)
			{
				if (strD.substr(strD.size() - 3, 3) == L"dds")
				{
					ThrowIfFailed(
						CreateDDSTextureFromFile(device, strD.c_str(), nullptr, this->texSRV.GetAddressOf())
					);
				}
				else
				{
					ThrowIfFailed(
						CreateWICTextureFromFile(device, strD.c_str(), nullptr, this->texSRV.GetAddressOf())
					);
				}
			}

			this->material = obj.material;
		}
	};

}

