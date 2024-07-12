#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"

class RadialBlur : public BaseEffect {
public:
	RadialBlur() = default;
	~RadialBlur() = default;

	static RadialBlur* GetInstance();

	void DrawCommand(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum);
private:
	void CreatePipelineState() override;
	
public:
	struct RadialParameter {
		Vector2 center;
		float blurWidth;
	};

	ConstantBuffer<RadialParameter> cParam_;

};