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
private:
	void CreateRenderTargetView();
	
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::unique_ptr<DescriptorHeap> rtvDescriptorHeap_;
	//	SwapChainからResourceを引っ張ってくる
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;
	
public: // ゲッター
	IDXGISwapChain4* const GetSwapChain() { return swapChain_.Get(); }
	ID3D12Resource* const GetBackBuffer() { return swapChainResources_[swapChain_->GetCurrentBackBufferIndex()].Get(); }
	DescriptorHeap* const GetRTVDesc() { return rtvDescriptorHeap_.get(); }
};
