#pragma once
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Graphics/Model/Model.h"
#include "MPEngine/Graphics/Animation/ModelAnimation.h"

class BaseComputeShader {
public:
	BaseComputeShader();
	~BaseComputeShader();

	virtual void Initialize(Model* model);
	void UpdateProcess(Model* model);

private:
	virtual void CreateRootSignature();
	virtual void CreatePipeline();

private:
	std::unique_ptr<RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipeline_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> outputVertexResource_ = nullptr;
	std::unique_ptr<DescriptorHandle> outputVertexHandle_;

};