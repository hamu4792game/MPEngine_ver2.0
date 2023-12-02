#include "DepthBuffer.h"
#include <cassert>
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

void DepthBuffer::Initialize(unsigned int width, unsigned int height) {
	dsvDescriptorHeap_ = std::make_unique<DescriptorHeap>();
	CreateDepthStencilTexture(width, height);
	dsvDescriptorHeap_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	//	DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//	Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;	//	2DTexture
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	//	DSVHeapの先頭にDSVを作る
	auto device = DeviceManager::GetInstance();
	device->GetDevice()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandle(0));
}

void DepthBuffer::CreateDepthStencilTexture(unsigned int width, unsigned int height) {
	//	生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;	//	textureの幅
	resourceDesc.Height = height;	//	textureの高さ
	resourceDesc.MipLevels = 1;	//	bitmapの数
	resourceDesc.DepthOrArraySize = 1;	//	奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;;	//	DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;	//	サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//	二次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//	DepthStencilとして使う通知

	//	利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	//	VRAM上に作る

	//	深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	//	1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;;	//	フォーマット。Resourceと合わせる

	//	Resourceの生成
	depthStencilResource_ = nullptr;
	auto device = DeviceManager::GetInstance();
	HRESULT hr = device->GetDevice()->CreateCommittedResource(
		&heapProperties,	//	Heapの設定
		D3D12_HEAP_FLAG_NONE,	//	Heapの特殊な設定。特になし
		&resourceDesc,	//	Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,	//	深度値を書き込む状態にしておく
		&depthClearValue,	//	Clear最適値
		IID_PPV_ARGS(&depthStencilResource_));	//	作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
}
