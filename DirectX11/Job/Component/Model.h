#pragma once

#include <vector>
#include <map>
#include "Utils/ShaderStructures.h"

using namespace std;

namespace Job {
	class Model
	{
	public:
		static const DirectX::XMMATRIX initMatrix;
		
	public:
		Model();
		Model(ID3D11Device* device);
		Model(ID3D11Device* device, ObjReader* obj);

		int AddObjPart(UINT numOfVertex, VertexPosNorTex* vertices,
			UINT numOfIndex, DXGI_FORMAT format, USHORT * indices, Materials material, std::wstring texName);
		void SetModelMatrix(int index, DirectX::XMMATRIX matrix);
		void SetWorldMatrix(int index, DirectX::XMMATRIX matrix);
		void SetRotateVector(float x, float y, float z);
		void TransformRotateVector(DirectX::XMMATRIX matrix);
		void SetGlobalMatrix(DirectX::XMMATRIX matrix);

		DirectX::XMMATRIX	GetMatrix(int index) { return m_modelMatrix[index] * m_worldMatrix[index] * m_globalMatrix; }
		bool				GetModelMode()			{ return m_modelMode; }
		vector<ObjModel>	GetObjModels()			{ return objModels; }
		DirectX::XMVECTOR	GetRotateVector()		{ return m_rotateVector; }
	
	private:
		Microsoft::WRL::ComPtr<ID3D11Device>							m_device;
		bool															m_modelMode;
		DirectX::XMVECTOR												m_rotateVector;
		vector<DirectX::XMMATRIX>										m_modelMatrix;
		vector<DirectX::XMMATRIX>										m_worldMatrix;
		DirectX::XMMATRIX												m_globalMatrix;
		vector<ObjModel>												objModels;
		map<wstring,Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	texSRVs;
	};
}

