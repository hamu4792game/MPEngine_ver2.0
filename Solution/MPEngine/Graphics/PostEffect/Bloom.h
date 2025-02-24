#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"
#include "IntermediateRenderTarget.h"

class Bloom : public BaseEffect {
public:
	Bloom();
	~Bloom() = default;

	static Bloom* GetInstance();

	void Initialize(SwapChain* swap);

	void DrawCommand(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum) override;
private:
	void CreatePipelineState() override;

public:
	struct BloomParameter {
		float value;
	};
	ConstantBuffer<BloomParameter> cParam_;

private:
	std::unique_ptr<IntermediateRenderTarget> render_;
};
