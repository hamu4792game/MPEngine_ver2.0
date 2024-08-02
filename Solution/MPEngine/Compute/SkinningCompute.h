#pragma once
#include "BaseComputeShader.h"

class SkinningCompute : public BaseComputeShader{
	SkinningCompute();
	~SkinningCompute();

	void Initialize(Model* model) override;

private:
	void CreateRootSignature() override;
	void CreatePipeline() override;

};