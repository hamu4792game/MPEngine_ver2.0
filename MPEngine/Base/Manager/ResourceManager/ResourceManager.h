#pragma once
#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"
#include <memory>
#include <cmath>
//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <string>

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
	DescriptorHeap* const GetSRVHeap() { return srvDescriptorHeap_.get(); }

public:
	// DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	// TextureResourceにデータを転送する
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	// Resourceを生成してreturnする
	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	// BufferResourceを作る関数
	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

};
