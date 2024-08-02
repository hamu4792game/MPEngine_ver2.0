#include "SkinningCompute.h"
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"

void SkinningCompute::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE range[1] = {};
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1; // 必要な数
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	const uint8_t paramIndex = 5;
	D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
	// Well
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = range;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(range);

	// Vertex
	range[0].BaseShaderRegister = 1;
	range[0].NumDescriptors = 1; // 必要な数
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = range;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(range);

	// VertexInfluence
	range[0].BaseShaderRegister = 2;
	range[0].NumDescriptors = 1; // 必要な数
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = range;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(range);

	// outputVertex
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1; // 必要な数
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = range;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(range);

	// ちょっとした情報
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[4].Descriptor.ShaderRegister = 0;

	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->CreateComputeRootSignature(rootParameter, paramIndex);
}

void SkinningCompute::CreatePipeline() {
	const std::string CSpath = "Skinning.CS.hlsl";
	Microsoft::WRL::ComPtr<IDxcBlob> computeShader = ShaderManager::GetInstance()->CompileShader(CSpath, ShaderManager::Compute);

	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};
	computePipelineStateDesc.CS = {
		.pShaderBytecode = computeShader->GetBufferPointer(),
		.BytecodeLength = computeShader->GetBufferSize()
	};
	computePipelineStateDesc.pRootSignature = rootSignature_->GetRootSignature().Get();
	HRESULT hr = DeviceManager::GetInstance()->GetDevice()->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&computePipeline_));
}
