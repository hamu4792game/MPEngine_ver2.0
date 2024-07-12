#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"

class Grayscale : public BaseEffect {
public:
	Grayscale() = default;
	~Grayscale() = default;

	static Grayscale* GetInstance();

	void DrawCommand(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum);
private:
	void CreatePipelineState() override;
	
public:
	struct GrayParameter {
		float grayscalevalue;
	};

	ConstantBuffer<GrayParameter> cParam_;

};