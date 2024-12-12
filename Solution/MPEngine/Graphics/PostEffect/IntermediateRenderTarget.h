#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"

// 中間renderターゲット
class IntermediateRenderTarget : public BaseEffect {
public:
	IntermediateRenderTarget() = default;
	IntermediateRenderTarget(class DeviceManager* device, class SwapChain* swapChain, class ResourceManager* rsManager);
	~IntermediateRenderTarget() = default;

	uint32_t PreProcess(ID3D12GraphicsCommandList* comList, uint32_t setHandleNumber, bool thisResource = true);
	uint32_t PostProcess();

private:
	void CreatePipelineState() override;
	SwapChain* swapchain_ptr = nullptr;


};