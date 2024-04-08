#pragma once
#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"
#include <memory>

class DepthBuffer {
public:
	DepthBuffer() = default;
	~DepthBuffer() = default;

	void Initialize(unsigned int width, unsigned int height);
	void Finalize();

private:
	// depthStencilResourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	std::unique_ptr<DescriptorHeap> dsvDescriptorHeap_;

private:
	void CreateDepthStencilTexture(unsigned int width, unsigned int height);
public:
	DescriptorHeap* const GetDSVDesc() { return dsvDescriptorHeap_.get(); }

};
