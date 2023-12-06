#include "GraphicsPipeline.h"

GraphicsPipeline* GraphicsPipeline::GetInstance() {
	static GraphicsPipeline instance;
	return &instance;
}

void GraphicsPipeline::CreatePipeline() {

}

void GraphicsPipeline::CreateSpritePipeline(BlendMode blendType) {

}

void GraphicsPipeline::CreateModelPipeline(BlendMode blendType) {

}
