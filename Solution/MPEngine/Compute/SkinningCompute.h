#pragma once
#include "BaseComputeShader.h"
#include "MPEngine/Base/ConstantBuffer.h"
#include "MPEngine/Graphics/Model/Model.h"

class SkinningCompute : public BaseComputeShader{
	SkinningCompute();
	~SkinningCompute();

	void Initialize(Model* model) override;
	void UpdateProcess() override;

private:
	void CreateRootSignature() override;
	void CreatePipeline() override;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> outputVertexResource_ = nullptr;
	DescriptorHandle outputVertexHandle_;
	
	struct SkinningInfomation {
		uint32_t numVertices;
	};
	ConstantBuffer<SkinningInfomation> cSkinningInfomation_;
	Model* model_;

};