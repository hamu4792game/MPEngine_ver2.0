#include "DescriptorHandle.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"

DescriptorHandle::DescriptorHandle() {
	cpuHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE(0);
	gpuHandle_ = D3D12_GPU_DESCRIPTOR_HANDLE(0);
}

void DescriptorHandle::CreateView(DescriptorHeap* heap, const uint32_t& heapNum) {
	cpuHandle_ = heap->GetCPUDescriptorHandle(heapNum);
	gpuHandle_ = heap->GetGPUDescriptorHandle(heapNum);
}
