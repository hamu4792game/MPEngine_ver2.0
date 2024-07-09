#pragma once
#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include <array>
#include "MPEngine/Graphics/RenderManager/RenderManager.h"

class RenderTarget {
public:
	RenderTarget() = default;
	~RenderTarget();

	// RenderTextureを作成し、そこに書き込むように設定する
	void CreateRenderTexture(class DeviceManager* device, class SwapChain* swapChain, class ResourceManager* rsManager);
	// 書かれたものをswapchainに書き込む(コピー)
	void DrawCommand(ID3D12GraphicsCommandList* comList);
	// 指定色でクリア
	void ClearRenderTarget(ID3D12GraphicsCommandList* comList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer) const;

	ID3D12Resource* const GetResource() { return renderTextureResource_.Get(); }
	const uint32_t& GetHandle() const { return rtvHandleNum_; }

	// レンダーターゲット用クリアカラー
	const float clearColor_[4] = { 0.1f,0.25f,0.5f,1.0f }; // 青っぽい色、RGBA
private:
	void CreatePipelineState();
	void CreateVertex(DeviceManager* device, ResourceManager* rsManager);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
	uint32_t handleNum_ = 0u;
	uint32_t rtvHandleNum_ = 0u;
	
	std::unique_ptr<RootSignature> rootSignature_;
	std::array<std::unique_ptr<GraphicsPipeline>, static_cast<int>(1)> graphicsPipeline_;

	// 描画必要情報
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
};
