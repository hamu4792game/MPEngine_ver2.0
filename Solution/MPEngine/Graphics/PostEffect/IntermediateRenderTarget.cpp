#include "IntermediateRenderTarget.h"
#include "MPEngine/Base/GraphicsManager/GraphicsManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"

IntermediateRenderTarget::IntermediateRenderTarget(DeviceManager* device, SwapChain* swapChain, ResourceManager* rsManager) {
	swapchain_ptr = swapChain;
	BaseEffect::CreateRenderTexture(device, swapChain, rsManager);
}

void IntermediateRenderTarget::CreatePipelineState() {
#pragma region Shader
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	const std::string VSpath = "Fullscreen.VS.hlsl";
	const std::string PSpath = "CopyImage.PS.hlsl";
	auto shaderInstance = ShaderManager::GetInstance();
	vertexShader = shaderInstance->CompileShader(VSpath, ShaderManager::ShaderType::Vertex);
	pixelShader = shaderInstance->CompileShader(PSpath, ShaderManager::ShaderType::Pixel);
#pragma endregion

#pragma region RootSignature
	D3D12_DESCRIPTOR_RANGE range[1] = {};
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1; // 必要な数
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	const uint8_t paramIndex = 1;
	D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = range;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(range);

	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->CreateRootSignature(rootParameter, paramIndex);
#pragma endregion

#pragma region GraphicsPipeline
	PipelineDesc plDesc;

	// 頂点データには何も入力しないので利用しない。ドライバやGUPが軽くなるかもしれないらしい
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	layoutDesc.pInputElementDescs = nullptr;
	layoutDesc.NumElements = 0;
	plDesc.layoutDesc_ = layoutDesc;

	plDesc.signature_ = rootSignature_->GetRootSignature().Get();
	plDesc.vertexShader_ = vertexShader.Get();
	plDesc.pixelShader_ = pixelShader.Get();

	plDesc.depthStencilDesc_.DepthEnable = false;
	// 裏面を表示しない
	plDesc.rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 塗りつぶす
	plDesc.rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	plDesc.rasterizerDesc_.DepthClipEnable = true;

	for (uint8_t i = 0; i < static_cast<uint8_t>(BlendMode::BlendCount); i++) {
		graphicsPipeline_.at(i) = std::make_unique<GraphicsPipeline>();
		graphicsPipeline_.at(i)->CreatePipeline(plDesc, static_cast<BlendMode>(i));
	}

#pragma endregion
}

uint8_t IntermediateRenderTarget::PreProcess(ID3D12GraphicsCommandList* comList, uint8_t setHandleNumber, bool thisResource) {
	if (thisResource) {
		GraphicsManager::CreateBarrier(renderTextureResource_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapchain_ptr->GetRTVHeap()->GetCPUDescriptorHandle(setHandleNumber);
	comList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
	return srvHandleNum_;
}

void IntermediateRenderTarget::PostProcess() {
	GraphicsManager::CreateBarrier(renderTextureResource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}


