#pragma once
#include <d3d12.h>
#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"

class DescriptorHandle {
public:
	DescriptorHandle();
	~DescriptorHandle() = default;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPU() const { return cpuHandle_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPU() const { return gpuHandle_; }

	void CreateView(DescriptorHeap* heap, const uint32_t& heapNum);
	
private:
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_;

};