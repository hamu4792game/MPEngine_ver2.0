#pragma once
#include <memory>
#include <wrl.h>
#include "MPEngine/Math/MathUtl.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "Graphics/Light/DirectionalLight.h"

class Camera3d;

// RenderManagerでのみ実体化
class ModelRender {
public:
	ModelRender() = default;
	~ModelRender() = default;

	// 初期化
	void Initialize();
	// spriteの描画
	void DrawCommand(Camera3d* cameraPtr = nullptr);

private:
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	std::unique_ptr<RootSignature> rootSignature_;
	std::array<std::unique_ptr<GraphicsPipeline>, static_cast<int>(BlendMode::BlendCount)> graphicsPipeline_;
	std::unique_ptr<RootSignature> skinningRootSignature_;
	std::array<std::unique_ptr<GraphicsPipeline>, static_cast<int>(BlendMode::BlendCount)> skinningGraphicsPipeline_;
	
	std::unique_ptr<DirectionalLight> directionalLight_;

};
