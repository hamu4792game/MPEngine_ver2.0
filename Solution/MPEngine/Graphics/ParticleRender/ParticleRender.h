#pragma once
#include "Math/MathUtl.h"
#include <memory>
#include <wrl.h>
#include "MPEngine/Math/MathUtl.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

class ParticleRender {
public:
	ParticleRender() = default;
	~ParticleRender() = default;

	// 初期化
	void Initialize();
	// particleの描画
	void DrawCommand(const Matrix4x4& viewProjectionMat, const Matrix4x4& billboardMat);

private:
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	std::unique_ptr<RootSignature> rootSignature_;
	std::array<std::unique_ptr<GraphicsPipeline>, static_cast<int>(BlendMode::BlendCount)> graphicsPipeline_;

};
