#pragma once
#include <list>
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
#include "MPEngine/Base/ConstantBuffer.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Graphics/Animation/ModelAnimation.h"

// モデル
class Model {
	friend class ModelRender;
	friend class ModelAnimation;
public:
	Model();
	~Model();
public: // setter
	// Textureのセット
	void SetModel(Object3d* model);
	void SetTexture(Texture* texture) { texture_ = texture; }
	void SetColor(const Vector4& color) { color_ = color; }
	void SetColor(const uint32_t& color) { color_ = ChangeColor(color); }
	void SetTransform(const WorldTransform& transform) { transform_ = transform; }
	void SetAnimation(ModelAnimation* animation) { animation_ = animation; }
private:
	void Initialize();
	void CreateVertexResource();
	void UploadVertexData();

public:
	bool isActive_ = true; // true:描画する/false:描画しない

private:
	static std::list<Model*> modelLists_;
	Object3d* model_ = nullptr;
	Texture* texture_ = nullptr;
	BlendMode blendType_ = BlendMode::Normal;
	Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	WorldTransform transform_; // 座標

	// 描画必要情報
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	struct TransformationMatrix {
		Matrix4x4 wvp;
		Matrix4x4 world;
		Matrix4x4 worldInverseTranspose;
	};
	ConstantBuffer<TransformationMatrix> cMat;
	struct Material {
		Vector4 color;
		int enableLighting = true; 
		float shininess = 0.0f; // 光沢度
		int phongLighing = false;
	};
	ConstantBuffer<Material> cMaterial;

	ModelAnimation* animation_ = nullptr;

};
