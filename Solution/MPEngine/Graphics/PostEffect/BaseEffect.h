#pragma once
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"

class BaseEffect {
public:
	BaseEffect();
	~BaseEffect();

	// RenderTextureを作成し、そこに書き込むように設定する
	void CreateRenderTexture(class DeviceManager* device, class SwapChain* swapChain, class ResourceManager* rsManager);
	void PreProcess();
	void PreDraw(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum);
	void DrawCommand(ID3D12GraphicsCommandList* comList);

	const uint32_t GetRTVHandle() const { return rtvHandleNum_; }
	const uint32_t GetSRVHandle() const { return srvHandleNum_; }

	void SetUsed(bool flag) { isUsed = flag; }

protected:
	virtual void CreatePipelineState() = 0;
	static void CreateCopyPipelineState();
	// 指定色でクリア
	void ClearRenderTarget(ID3D12GraphicsCommandList* comList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer) const;

	std::unique_ptr<RootSignature> rootSignature_;
	std::array<std::unique_ptr<GraphicsPipeline>, static_cast<int>(BlendMode::BlendCount)> graphicsPipeline_;

	static std::unique_ptr<RootSignature> copyRootSignature_;
	static std::unique_ptr<GraphicsPipeline> copyGraphicsPipeline_;

	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;
	uint32_t srvHandleNum_ = 0u;
	uint32_t rtvHandleNum_ = 0u;

	BlendMode blendType_ = BlendMode::Normal;
	bool isUsed = false;

	// レンダーターゲット用クリアカラー
	const float clearColor_[4] = { 0.1f,0.25f,0.5f,1.0f }; // 青っぽい色、RGBA
};
