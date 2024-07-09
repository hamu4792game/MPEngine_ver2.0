#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"

class Grayscale : public BaseEffect {
public:
	Grayscale() = default;
	~Grayscale() = default;

	void DrawCommand(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum);
private:
	void CreatePipelineState() override;
	
public:
	struct GrayParameter {
		Vector2 center;
		float blurWidth;
	};

	ConstantBuffer<GrayParameter> cParam_;
	const uint32_t GetRTVHandle() { return rtvHandleNum_; }

};