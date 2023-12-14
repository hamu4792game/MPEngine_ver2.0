#include "PipelineManager.h"

PipelineManager* PipelineManager::GetInstance() {
	static PipelineManager instance;
	return &instance;
	return nullptr;
}

void PipelineManager::Initialize() {

}
