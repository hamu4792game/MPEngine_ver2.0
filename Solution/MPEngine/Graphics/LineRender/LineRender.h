#pragma once
#include "Math/MathUtl.h"
#include <memory>
#include <wrl.h>
#include "MPEngine/Math/MathUtl.h"
#include "MPEngine/Base/DetailSetting/RootSignature/RootSignature.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "MPEngine/Base/DetailSetting/DescriptorHandle/DescriptorHandle.h"

class Line;
class LineRender {
public:
	LineRender() = default;
	~LineRender();

	// 初期化
	void Initialize();
	// particleの描画
	void DrawCommand(const Matrix4x4& viewProjectionMat);

private:
	void CreateVertexResource();
	UINT UploadVertexData(std::list<Line*> lineList); // activeなlineの数を返す
	void CreateInstancing();
	void UploadInstancingData(std::list<Line*> lineList, const Matrix4x4& viewProjectionMat);
private:
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	std::unique_ptr<RootSignature> rootSignature_;
	std::unique_ptr<GraphicsPipeline> graphicsPipeline_;
	// 描画必要情報
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	// Instancing用のリソースを作る
	const uint32_t kMaxNumInstance_ = 1000u; // 描画制限
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_ = nullptr;
	DescriptorHandle instancingSrvHandle_;
	// instancing描画に必要な定数バッファをまとめたもの
	struct InstancingStruct {
		Matrix4x4 wvp;
		Vector4 color;
	};
};
