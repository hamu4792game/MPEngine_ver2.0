#pragma once
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include <array>
#include <memory>

class PipelineManager {
private:
	PipelineManager() = default;
	~PipelineManager() = default;
public:
	static PipelineManager* GetInstance();

	void Initialize();

private: // メンバ変数
	std::array<GraphicsPipeline,static_cast<int>(BlendMode::BlendCount)> spritePipelineState;
	std::array<GraphicsPipeline,static_cast<int>(BlendMode::BlendCount)> modelPipelineState;

};
