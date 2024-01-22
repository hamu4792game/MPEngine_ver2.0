#pragma once
//	DirextXtex
#include "externals/DirectXTex/d3dx12.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include <wrl.h>

// ブレンドモード
enum class BlendMode {
	None,   // ブレンドなし
	Normal, // 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	Add,    // 加算。Src * SrcA + Dest * 1
	Subtract, // 減算。Dest * 1 - Src * SrcA
	Multily,  // 乗算。Src * 0 + Dest * Src
	Screen,   // スクリーン。Src * (1 - Dest) + Dest * 1
	Dark,	// 比較暗
	Light,	// 比較明

	BlendCount,
};

struct PipelineDesc {
	D3D12_INPUT_LAYOUT_DESC layoutDesc_{};
	ID3D12RootSignature* signature_;
	IDxcBlob* vertexShader_;
	IDxcBlob* pixelShader_;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
};

class GraphicsPipeline {
public:
	GraphicsPipeline() = default;
	~GraphicsPipeline() = default;

	// 生成
	void CreatePipeline(PipelineDesc pipelineDesc, BlendMode type);

public: // ゲッター
	ID3D12PipelineState* GetPipelineState() const { return pipelineState_.Get(); }

private: // メンバ変数
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

};

