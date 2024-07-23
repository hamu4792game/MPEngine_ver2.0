#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"

class HSVFilter : public BaseEffect {
public:
	HSVFilter() = default;
	~HSVFilter() = default;

	static HSVFilter* GetInstance();

	void DrawCommand(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum);
private:
	void CreatePipelineState() override;
	
public:
	struct HSVParameter {
		float hue = 0.0f;
		float saturation = 0.0f;
		float value = 0.0f;
	};

	ConstantBuffer<HSVParameter> cParam_;

};