#pragma once

#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"
#include <memory>
#include <array>

class SwapChain {
public:
	SwapChain() = default;
	~SwapChain();

	void Finalize();

	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue);

	//	GPUとOSに画面の交換を行うよう通知する
	void Present();

	const uint32_t GetCount();

private:
	void CreateRenderTargetView();
	
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::unique_ptr<DescriptorHeap> rtvDescriptorHeap_;
	//	SwapChainからResourceを引っ張ってくる
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;
	uint32_t renderCount_ = 2u; // 今のRTが追加された数 0と1はswapchainで使用
	
public: // ゲッター
	IDXGISwapChain4* const GetSwapChain() { return swapChain_.Get(); }
	ID3D12Resource* const GetBackBuffer() { return swapChainResources_[swapChain_->GetCurrentBackBufferIndex()].Get(); }
	DescriptorHeap* const GetRTVHeap() { return rtvDescriptorHeap_.get(); }
};
