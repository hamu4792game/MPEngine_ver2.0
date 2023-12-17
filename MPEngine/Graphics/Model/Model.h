#pragma once
#include <list>
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
#include "MPEngine/Base/ConstantBuffer.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

// モデル
class Model {
	friend class ModelRender;
public:
	Model();
	~Model();
	void Initialize();
public:
	// Textureのセット
	void SetModel(const std::shared_ptr<Object3d>& model);
	void SetTexture(const std::shared_ptr<Texture>& texture) { texture_ = texture; }

private:
	void CreateVertexResource();
	void UploadVertexData();

public:
	Vector3 scale_;
	Vector3 rotate_;
	Vector3 translate_;
	bool isActive_ = true;

private:
	static std::list<Model*> modelLists_;
	std::shared_ptr<Object3d> model_;
	std::shared_ptr<Texture> texture_;
	BlendMode blendType_ = BlendMode::Normal;
	Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

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
