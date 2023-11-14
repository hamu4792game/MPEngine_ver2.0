#include "DescriptorHeap.h"
#include <cassert>
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

ID3D12DescriptorHeap* DescriptorHeap::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	heapType_ = heapType;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	auto device = DeviceManager::GetInstance()->GetDevice();
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap_.GetAddressOf()));
	//	ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(unsigned int index) {
	unsigned long long size = ((DeviceManager::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(heapType_)) * static_cast<unsigned long long>(index));
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += size;
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(unsigned int index) {
	unsigned long long size = (DeviceManager::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(heapType_)) * static_cast<unsigned long long>(index);
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += size;
	return handleGPU;
}
