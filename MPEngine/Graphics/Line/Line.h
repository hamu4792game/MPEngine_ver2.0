#pragma once
#include <list>
#include "MPEngine/Base/ConstantBuffer.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "Utils/WorldTransform/WorldTransform.h"

// モデル
class Line {
	friend class LineRender;
public:
	Line();
	~Line();
public:
	void SetColor(const Vector4& color) { color_ = color; }
	void SetColor(const uint32_t& color) { color_ = ChangeColor(color); }
	void SetLine(const Vector3& start, const Vector3& end);
private:
	void Initialize();
	void CreateVertexResource();
	void UploadVertexData();

public:
	bool isActive_ = true;

private:
	static std::list<Line*> lineLists_;
	Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Vector3 start_; // 始点
	Vector3 end_; // 終点

	// 描画必要情報
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	struct TransformationMatrix {
		Matrix4x4 wvp;
	};
	ConstantBuffer<TransformationMatrix> cMat;
	struct Material {
		Vector4 color;
	};
	ConstantBuffer<Material> cMaterial;

};
