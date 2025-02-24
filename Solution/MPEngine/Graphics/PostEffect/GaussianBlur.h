#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"

class GaussianBlur : public BaseEffect {
public:
	GaussianBlur() = default;
	~GaussianBlur() = default;

	static GaussianBlur* GetInstance();

	void DrawCommand(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum) override;
private:
	void CreatePipelineState() override;

public:
	struct GaussianBlurParameter {
		int32_t value;
	};

	ConstantBuffer<GaussianBlurParameter> cParam_;
};
