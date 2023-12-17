#pragma once
#include <memory>
#include <dxcapi.h>
#include <wrl.h>
#include "MPEngine/Math/MathUtl.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

// RenderManagerでのみ実体化
class ModelRender {
public:
	ModelRender() = default;
	~ModelRender() = default;

	// 初期化
	void Initialize();
	// spriteの描画
	void DrawCommand(const Matrix4x4& viewProjectionMat);

private:
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	std::unique_ptr<RootSignature> rootSignature_;
	std::array<std::unique_ptr<GraphicsPipeline>, static_cast<int>(BlendMode::BlendCount)> graphicsPipeline_;

};
