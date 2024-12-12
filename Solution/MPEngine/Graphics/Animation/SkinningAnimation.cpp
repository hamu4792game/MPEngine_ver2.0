#include "SkinningAnimation.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

void SkinningAnimation::CreatePipeline() {
	const std::string CSpath = "Skinning.CS.hlsl";
	auto shaderInstance = ShaderManager::GetInstance();
	Microsoft::WRL::ComPtr<IDxcBlob> computeShader = shaderInstance->CompileShader(CSpath, ShaderManager::ShaderType::Vertex);

#pragma region RootSignature
	D3D12_DESCRIPTOR_RANGE range[1] = {};
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1;	//	必要な数
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	const uint8_t paramIndex = 5;
	D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = range;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(range);


	RootSignature rootsignature;
	rootsignature.CreateRootSignature(rootParameter, paramIndex);

#pragma endregion

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
		.pShaderBytecode = computeShader->GetBufferPointer(),
		.BytecodeLength = computeShader->GetBufferSize()
	};
	computePipelineStateDesc.pRootSignature = rootsignature.GetRootSignature().Get();
	Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState = nullptr;
	HRESULT hr = DeviceManager::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipelineState));
	SUCCEEDED(hr);
}
