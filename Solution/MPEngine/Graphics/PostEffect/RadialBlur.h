#pragma once
#include "BaseEffect.h"
#include "MPEngine/Base/ConstantBuffer.h"

class RadialBlur : public BaseEffect {
public:
	RadialBlur() = default;
	~RadialBlur() = default;

	void DrawCommand(ID3D12GraphicsCommandList* comList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer);
private:
	void CreatePipelineState() override;
	
public:
	struct RadialParameter {
		Vector2 center;
		float blurWidth;
	};

	ConstantBuffer<RadialParameter> cParam_;
	const uint32_t GetRTVHandle() { return rtvHandleNum_; }

};