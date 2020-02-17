#pragma once

#include <vector>
#include <map>
#include "Utils/ShaderStructures.h"

using namespace std;

namespace Job {
	class Model
	{
	public:
		Model() {};
		Model(ID3D11Device* device, ObjReader* obj);
		vector<ObjModel> GetObjModels() { return objModels; }

	private:
		vector<ObjModel> objModels;
		map<wstring,Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texSRVs;
	};
}

