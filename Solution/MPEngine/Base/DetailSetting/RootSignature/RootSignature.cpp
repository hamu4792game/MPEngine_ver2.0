#include "RootSignature.h"
#include "MPEngine/Base/Log.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

void RootSignature::CreateRootSignature(D3D12_ROOT_PARAMETER* rootParameter, UINT num) {
	// ルートシグネチャーの作成
	D3D12_ROOT_SIGNATURE_DESC sigDesc{};
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	sigDesc.pParameters = rootParameter;
	sigDesc.NumParameters = num;

	D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSampler[0].ShaderRegister = 0;
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	sigDesc.pStaticSamplers = staticSampler;
	sigDesc.NumStaticSamplers = _countof(staticSampler);

	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元に生成
	rootSignature_ = nullptr;
	auto device = DeviceManager::GetInstance();
	hr = device->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (errorBlob) {
		errorBlob->Release();
	}
	signatureBlob->Release();
}
