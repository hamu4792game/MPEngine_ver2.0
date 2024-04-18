#include "RenderTarget.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

RenderTarget::~RenderTarget() {
	if (renderTextureResource_) {
		renderTextureResource_->Release();
	}
}

void RenderTarget::CreateRenderTexture(DeviceManager* device, SwapChain* swapChain, ResourceManager* rsManager) {
	const Vector4 color = { 0.1f,0.25f,0.5f,1.0f };
	renderTextureResource_ = rsManager->CreateRenderTextureResource(device->GetDevice(), swapChain->GetBackBuffer()->GetDesc(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, color);

	// RTVの設定 swapChainと同じ
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む
	// RTVの生成
	device->GetDevice()->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc, swapChain->GetRTVHeap()->GetCPUDescriptorHandle(2));

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	handleNum_ = rsManager->GetCount();
	// SRVの生成
	device->GetDevice()->CreateShaderResourceView(renderTextureResource_.Get(), &srvDesc, rsManager->GetSRVHeap()->GetCPUDescriptorHandle(handleNum_));
}

void RenderTarget::DrawColl(ID3D12CommandList* comList) {

}
