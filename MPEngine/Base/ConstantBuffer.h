#pragma once
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

//	テンプレート化
template<class T>
class ConstantBuffer {
public:
	ConstantBuffer() : resource(nullptr),cBufferViewDesc(),pData(nullptr) {
		auto rsManager = ResourceManager::GetInstance();
		resource = rsManager->CreateBufferResource(DeviceManager::GetInstance()->GetDevice(), (sizeof(T) + 0xff) & ~0xff);
		cBufferViewDesc.BufferLocation = resource->GetGPUVirtualAddress();
		cBufferViewDesc.SizeInBytes = UINT(resource->GetDesc().Width);
		
		resource->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	}
	
	~ConstantBuffer() {
		resource->Release();
	}

public:

	//	ポインタを返す
	T* operator->() const {
		return pData;
	}
	T& operator*() const {
		return *pData;
	}

	//	デスクリプタヒープの場所を取得
	void CreateView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) {
		DeviceManager::GetInstance()->GetDevice()->CreateConstantBufferView(&cBufferViewDesc, descriptorHandle);
	}

	//	仮想アドレスのget
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() { return resource->GetGPUVirtualAddress(); };

private:
	ID3D12Resource* resource = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cBufferViewDesc{};
	// 転送用
	T* pData;
};

