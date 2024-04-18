#pragma once
#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"


class RenderTarget {
public:
	RenderTarget() = default;
	~RenderTarget();

	// RenderTextureを作成し、そこに書き込むように設定する
	void CreateRenderTexture(class DeviceManager* device, class SwapChain* swapChain, class ResourceManager* rsManager);
	// 書かれたものをswapchainに書き込む(コピー)
	void DrawColl(ID3D12CommandList* comList);

	ID3D12Resource* const GetResource() { return renderTextureResource_.Get(); }
	const uint32_t& GetHandle() const { return handleNum_; }
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
	uint32_t handleNum_ = 0u;

};
