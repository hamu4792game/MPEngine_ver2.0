#pragma once
//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

#include <wrl.h>

class RootSignature {
public:
	RootSignature() = default;
	~RootSignature() = default;
	// RootSignatureの生成
	void CreateRootSignature(D3D12_ROOT_PARAMETER* rootParameter, UINT num);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature_; }

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};
