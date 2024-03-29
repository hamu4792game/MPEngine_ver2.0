#pragma once
#include "Math/MathUtl.h"
#include <memory>
#include <wrl.h>
#include "MPEngine/Math/MathUtl.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

class LineRender {
public:
	LineRender() = default;
	~LineRender() = default;

	// 初期化
	void Initialize();
	// particleの描画
	void DrawCommand(const Matrix4x4& viewProjectionMat);

private:
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<GraphicsPipeline> graphicsPipeline_;

};
