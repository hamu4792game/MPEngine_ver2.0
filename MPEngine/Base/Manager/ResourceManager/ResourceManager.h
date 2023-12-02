#pragma once
#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"
#include <memory>
#include <cmath>

class ResourceManager {
private:
	ResourceManager() = default;
	~ResourceManager() = default;

public:
	static ResourceManager* GetInstance();
	void Initialize();
	void Finalize();

private:
	std::unique_ptr<DescriptorHeap> srvDescriptorHeap_;
	const unsigned int kDescriptorHeapSize_ = 4096;

public:
	ID3D12DescriptorHeap* const GetSRVHeap() { return srvDescriptorHeap_->GetDescriptorHeap(); }

};
