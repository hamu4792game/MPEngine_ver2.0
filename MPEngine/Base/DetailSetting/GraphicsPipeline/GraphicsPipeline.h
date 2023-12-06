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

class GraphicsPipeline {
private:
	GraphicsPipeline() = default;
	~GraphicsPipeline() = default;
public:
	static GraphicsPipeline* GetInstance();

	// 初期化
	void CreatePipeline();

private: // メンバ変数
	Microsoft::WRL::ComPtr<ID3D12PipelineState> spritePipelineState[static_cast<int>(BlendMode::BlendCount)];
	Microsoft::WRL::ComPtr<ID3D12PipelineState> modelPipelineState[static_cast<int>(BlendMode::BlendCount)];

private:
	void CreateSpritePipeline(BlendMode blendType);
	void CreateModelPipeline(BlendMode blendType);

};