#include "pch.h"
#include "Utils/ObjReader.h"
#include "Model.h"

Job::Model::Model(ID3D11Device * device, ObjReader * objReader)
{
	objModels.resize(objReader->objParts.size());

	for (int i = 0; i < objModels.size(); i++) 
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
			device->CreateBuffer(&vbd, &InitData, objModels[i].vertexBuffer.ReleaseAndGetAddressOf())
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
			device->CreateBuffer(&ibd, &InitData, objModels[i].indexBuffer.ReleaseAndGetAddressOf())
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
						CreateDDSTextureFromFile(device, strD.c_str(), nullptr, texSRV.GetAddressOf())
					);
				}
				else
				{
					ThrowIfFailed(
						CreateWICTextureFromFile(device, strD.c_str(), nullptr, texSRV.GetAddressOf())
					);
				}
				objModels[i].texSRV = texSRV;
				texSRVs.insert(pair<wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>(strD, texSRV));
			}
		}

		objModels[i].material = obj.material;
	}

	
}
