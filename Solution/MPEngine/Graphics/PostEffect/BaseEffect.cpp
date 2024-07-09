#include "BaseEffect.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "ResourceManager/ResourceManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"
#include "Base/GraphicsManager/GraphicsManager.h"

BaseEffect::BaseEffect() {

}

BaseEffect::~BaseEffect() {
	if (renderTextureResource_) {
		renderTextureResource_->Release();
	}
}

void BaseEffect::CreateRenderTexture(DeviceManager* device, SwapChain* swapChain, ResourceManager* rsManager) {
	renderTextureResource_ = rsManager->CreateRenderTextureResource(device->GetDevice(), swapChain->GetBackBuffer()->GetDesc(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Vector4(clearColor_));

	// RTVの設定 swapChainと同じ
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む
	rtvHandleNum_ = swapChain->GetCount();
	// RTVの生成
	device->GetDevice()->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc, swapChain->GetRTVHeap()->GetCPUDescriptorHandle(rtvHandleNum_));

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	handleNum_ = rsManager->GetCount();
	// SRVの生成
	device->GetDevice()->CreateShaderResourceView(renderTextureResource_.Get(), &srvDesc, rsManager->GetSRVHeap()->GetCPUDescriptorHandle(handleNum_));

	// 必要パラメーターの生成
	CreatePipelineState();

	GraphicsManager::CreateBarrier(renderTextureResource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void BaseEffect::PreProcess() {
	GraphicsManager::CreateBarrier(renderTextureResource_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void BaseEffect::PreDraw(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum) {
	comList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	comList->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
	comList->SetPipelineState(graphicsPipeline_[static_cast<uint32_t>(blendType_)]->GetPipelineState());
	// 読み込むテクスチャの設定
	comList->SetGraphicsRootDescriptorTable(0, ResourceManager::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandle(handleNum));
}

void BaseEffect::DrawCommand(ID3D12GraphicsCommandList* comList) {	
	// 頂点3つ描画
	comList->DrawInstanced(3, 1, 0, 0);
}

void BaseEffect::ClearRenderTarget(ID3D12GraphicsCommandList* comList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer) const {
	// 指定した色で画面全体をクリアする
	comList->ClearRenderTargetView(rtvHeapPointer, clearColor_, 0, nullptr);
}
