#include "ResourceManager.h"

ResourceManager* ResourceManager::GetInstance() {
	static ResourceManager instance;
	return &instance;
}

void ResourceManager::Initialize() {
	srvDescriptorHeap_ = std::make_unique<DescriptorHeap>();
	srvDescriptorHeap_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kDescriptorHeapSize_, true);

}

void ResourceManager::Finalize() {
	srvDescriptorHeap_->Release();
}
