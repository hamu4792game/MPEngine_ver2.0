#pragma once
#include <memory>
#include <wrl.h>
#include "MPEngine/Math/Matrix4x4.h"
#include <dxcapi.h>
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

// RenderManagerでのみ実体化
class SkyBoxRender {
public:
	SkyBoxRender() = default;
	~SkyBoxRender() = default;

	// 初期化
	void Initialize();
	// SkyBoxの描画
	void DrawCommand(const Matrix4x4& viewProjectionMat);

private:
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<GraphicsPipeline> graphicsPipeline_;

};
