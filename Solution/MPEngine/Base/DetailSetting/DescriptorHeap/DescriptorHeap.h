#pragma once

//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")

#include <Windows.h>
#include <wrl.h>
#include <cstdint>

class DescriptorHeap {
public:
	DescriptorHeap() = default;
	~DescriptorHeap() = default;

	//	DescriptorHeap作成の関数
	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors, bool shaderVisible);

	void Release();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	D3D12_DESCRIPTOR_HEAP_TYPE heapType_;

public:
	ID3D12DescriptorHeap* const GetDescriptorHeap() { return descriptorHeap_.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	
};
