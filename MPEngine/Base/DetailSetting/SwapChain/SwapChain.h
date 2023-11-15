#pragma once

#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"
#include <memory>

class SwapChain
{
public:
	SwapChain() = default;
	~SwapChain() = default;

	static SwapChain* GetInstance();

	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue);

	void CreateRenderTargetView();
	//	GPUとOSに画面の交換を行うよう通知する
	void Present();
	
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::unique_ptr<DescriptorHeap> rtvDescriptorHeap_;
	//	SwapChainからResourceを引っ張ってくる
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2]{ nullptr };
	
public: // ゲッター
	IDXGISwapChain4* const GetSwapChain() { return swapChain_.Get(); }

};
