#include "SwapChain.h"
#include <cassert>
#include "MPEngine/Base/WindowSupervisor/WindowSupervisor.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

SwapChain::~SwapChain() {
	Finalize();
}

void SwapChain::Finalize() {
	if (rtvDescriptorHeap_) {
		rtvDescriptorHeap_->Release();
	}
}

void SwapChain::CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue) {
	swapChain_ = nullptr;
	auto hWnd = WindowSupervisor::GetInstance()->GetHwnd();
	RECT clientRect{};
	if (!GetClientRect(hWnd, &clientRect)) {
		assert(false);
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = clientRect.right - clientRect.left;	//	画面の幅
	swapChainDesc.Height = clientRect.bottom - clientRect.top;	//	画面の高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//	色の形式
	swapChainDesc.SampleDesc.Count = 1;		//	マルチサンプリングしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//	描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;			//	ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//	モニタに移したら、中身を破棄
	//	コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hWnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	CreateRenderTargetView();
}

void SwapChain::CreateRenderTargetView() {
	HRESULT hr = S_FALSE;
	//	ディスクリプタヒープの生成
	rtvDescriptorHeap_ = std::make_unique<DescriptorHeap>();
	//	RTV用のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	rtvDescriptorHeap_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	
	//	SwapChainからResourceを引っ張ってくる
	swapChainResources_[0] = nullptr;
	swapChainResources_[1] = nullptr;
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(swapChainResources_[0].GetAddressOf()));
	//	うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(swapChainResources_[1].GetAddressOf()));
	assert(SUCCEEDED(hr));

	//	RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;		//	出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;	//	2dテクスチャとして書き込む
	//	ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvNumberHandle = rtvDescriptorHeap_->GetCPUDescriptorHandle(0);
	//	RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[2]{};
	auto device = DeviceManager::GetInstance();
	//	まず1つ目を作る。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandle[0] = rtvNumberHandle;
	device->GetDevice()->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc, rtvHandle[0]);
	//	2つ目のディスクリプタハンドルを得る（自力で）
	rtvNumberHandle = rtvDescriptorHeap_->GetCPUDescriptorHandle(1);
	rtvHandle[1] = rtvNumberHandle;
	//	2つ目を作る
	device->GetDevice()->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc, rtvHandle[1]);
}

void SwapChain::Present() {
	swapChain_->Present(1, 0);
}

