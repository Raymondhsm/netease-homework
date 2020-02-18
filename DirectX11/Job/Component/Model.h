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

		DirectX::XMMATRIX GetModelMatrix(int index) { return m_modelMatrix[index]; }
		bool GetModelMode() { return m_modelMode; }
		vector<ObjModel> GetObjModels() { return objModels; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>							m_device;
		bool															m_modelMode;
		vector<DirectX::XMMATRIX>										m_modelMatrix;
		vector<ObjModel>												objModels;
		map<wstring,Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	texSRVs;
	};
}

