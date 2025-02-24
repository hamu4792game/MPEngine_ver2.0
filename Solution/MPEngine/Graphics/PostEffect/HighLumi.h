#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"

class HighLumi : public BaseEffect {
public:
	HighLumi() = default;
	~HighLumi() = default;

	static HighLumi* GetInstance();

	void DrawCommand(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum) override;
private:
	void CreatePipelineState() override;

public:
	struct HighLumiParameter {
		float value;
	};

	ConstantBuffer<HighLumiParameter> cParam_;
};
