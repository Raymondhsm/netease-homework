#include "pch.h"
#include "Utils/ObjReader.h"
#include "Model.h"

const XMMATRIX Job::Model::initMatrix = 
{
			1.f,0.f,0.f,0.f,
			0.f,1.f,0.f,0.f,
			0.f,0.f,1.f,0.f,
			0.f,0.f,0.f,1.f
};

Job::Model::Model() :
	m_modelMode(false),
	m_rotateVector(XMVectorSet(0.f,0.f,0.f,0.f)),
	m_globalMatrix(initMatrix)
{
}

Job::Model::Model(ID3D11Device * device):
	m_device(device),
	m_modelMode(false),
	m_rotateVector(XMVectorSet(0.f, 0.f, 0.f, 0.f)),
	m_globalMatrix(initMatrix)
{
}

Job::Model::Model(ID3D11Device * device, ObjReader * objReader):
	m_device(device),
	m_modelMode(false)
{
	objModels.resize(objReader->objParts.size());

	for (UINT i = 0; i < objModels.size(); i++) 
	{
		auto obj = objReader->objParts[i];
		objModels[i].vertexCount = (UINT)obj.vertices.size();

		// 设置顶点缓冲区描述
		D3D11_BUFFER_DESC vbd;
		ZeroMemory(&vbd, sizeof(vbd));
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = objModels[i].vertexCount * (UINT)sizeof(VertexPosNormalTex);
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;

		// 新建顶点缓冲区
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = obj.vertices.data();
		ThrowIfFailed(
			m_device->CreateBuffer(&vbd, &InitData, objModels[i].vertexBuffer.ReleaseAndGetAddressOf())
		);

		// 设置索引缓冲区描述
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		if (objModels[i].vertexCount > 65535)
		{
			objModels[i].indexCount = (UINT)obj.indices32.size();
			objModels[i].indexFormat = DXGI_FORMAT_R32_UINT;
			ibd.ByteWidth = objModels[i].indexCount * (UINT)sizeof(DWORD);
			InitData.pSysMem = obj.indices32.data();

		}
		else
		{
			objModels[i].indexCount = (UINT)obj.indices16.size();
			objModels[i].indexFormat = DXGI_FORMAT_R16_UINT;
			ibd.ByteWidth = objModels[i].indexCount * (UINT)sizeof(WORD);
			InitData.pSysMem = obj.indices16.data();
		}
		// 新建索引缓冲区
		ThrowIfFailed(
			m_device->CreateBuffer(&ibd, &InitData, objModels[i].indexBuffer.ReleaseAndGetAddressOf())
		);


		// 创建漫射光对应纹理
		auto& strD = obj.texStrDiffuse;
		if (strD.size() > 4)
		{
			auto it = texSRVs.find(strD);
			if (it != texSRVs.end())
			{
				objModels[i].texSRV = it->second;
			}
			else
			{
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texSRV;
				if (strD.substr(strD.size() - 3, 3) == L"dds")
				{
					ThrowIfFailed(
						CreateDDSTextureFromFile(m_device.Get(), strD.c_str(), nullptr, texSRV.GetAddressOf())
					);
				}
				else
				{
					ThrowIfFailed(
						CreateWICTextureFromFile(m_device.Get(), strD.c_str(), nullptr, texSRV.GetAddressOf())
					);
				}
				objModels[i].texSRV = texSRV;
				texSRVs.insert(pair<wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(strD, texSRV));
			}
		}

		objModels[i].material = obj.material;
	}

	
}

int Job::Model::AddObjPart(UINT numOfVertex, VertexPosNorTex* vertices,
	UINT numOfIndex, DXGI_FORMAT format, USHORT * indices, Materials material, std::wstring texName)
{
	ObjModel objModel;

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = numOfVertex * (UINT)sizeof(VertexPosNorTex);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	ThrowIfFailed(
		m_device->CreateBuffer(&vbd, &InitData, objModel.vertexBuffer.ReleaseAndGetAddressOf())
	);

	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.ByteWidth = numOfIndex * (UINT)sizeof(USHORT);
	InitData.pSysMem = indices;

	// 新建索引缓冲区
	ThrowIfFailed(
		m_device->CreateBuffer(&ibd, &InitData, objModel.indexBuffer.ReleaseAndGetAddressOf())
	);

	objModel.indexCount = numOfIndex;
	objModel.indexFormat = format;

	// 创建漫射光对应纹理
	auto& strD = texName;
	if (strD.size() > 4)
	{
		auto it = texSRVs.find(strD);
		if (it != texSRVs.end())
		{
			objModel.texSRV = it->second;
		}
		else
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texSRV;
			if (strD.substr(strD.size() - 3, 3) == L"dds")
			{
				ThrowIfFailed(
					CreateDDSTextureFromFile(m_device.Get(), strD.c_str(), nullptr, texSRV.GetAddressOf())
				);
			}
			else
			{
				ThrowIfFailed(
					CreateWICTextureFromFile(m_device.Get(), strD.c_str(), nullptr, texSRV.GetAddressOf())
				);
			}
			objModel.texSRV = texSRV;
			texSRVs.insert(pair<wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(strD, texSRV));
		}
	}

	objModel.material = material;

	objModels.push_back(objModel);
	m_modelMatrix.push_back(initMatrix);
	m_worldMatrix.push_back(initMatrix);
	return objModels.size() - 1;
}

void Job::Model::SetModelMatrix(UINT index, DirectX::XMMATRIX matrix)
{
	if (index < 0 || index >= objModels.size())
	{
		return;
	}
	else
	{
		m_modelMatrix[index] = m_modelMatrix[index] * matrix;
	}
}

void Job::Model::SetWorldMatrix(UINT index, DirectX::XMMATRIX matrix)
{
	if (index < 0 || index >= objModels.size())
	{
		return;
	}
	else
	{
		m_worldMatrix[index] = m_worldMatrix[index] * matrix;
	}
}

void Job::Model::SetRotateVector(float x, float y, float z)
{
	m_rotateVector = XMVectorSet(x, y, z, 0.f);
}

void Job::Model::TransformRotateVector(DirectX::XMMATRIX matrix)
{
	m_rotateVector = XMVector3Transform(m_rotateVector, matrix);
}

void Job::Model::SetGlobalMatrix(DirectX::XMMATRIX matrix)
{
	m_globalMatrix = m_globalMatrix * matrix;
}

