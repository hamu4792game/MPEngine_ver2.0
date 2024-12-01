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
	void SetModel(Object3d* model);
	/// <summary>
	/// Textureのセッター
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="number">変更したいモデルテクスチャの番号、-1の場合は全部変わる</param>
	void SetTexture(Texture* texture, const int32_t number = 0);
	void SetColor(const Vector4& color) { materials.color = color; }
	void SetColor(const uint32_t& color) { materials.color = ChangeColor(color); }
	void SetTransform(const WorldTransform& transform) { transform_ = transform; }
	void SetAnimation(ModelAnimation* animation) { animation_ = animation; }
	void SetBlendType(BlendMode type) { blendType_ = type; }
	void SetUVMatrix(const Matrix4x4& uvMat) { cMaterial->uvMatrix = uvMat; }

	const WorldTransform& GetTransform() const { return transform_; }
	ModelAnimation* GetAnimation() const { return animation_; }
private:
	

public:
	bool isActive_ = true; // true:描画する/false:描画しない

private:
	static std::list<Model*> modelLists_;
	Object3d* model_ = nullptr;
	std::vector<Texture*> texture_;
	BlendMode blendType_ = BlendMode::Normal;
	//Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	WorldTransform transform_; // 座標

	struct TransformationMatrix {
		Matrix4x4 wvp;
		Matrix4x4 world;
		Matrix4x4 worldInverseTranspose;
	};
	ConstantBuffer<TransformationMatrix> cMat;
	struct Material {
		Vector4 color = Vector4::one;
		int enableLighting = true; 
		float shininess = 5.0f; // 光沢度
		int phongLighing = false;
		float environmentCoefficient = 0.0f; // 環境マップ
		Matrix4x4 uvMatrix = Matrix4x4(Vector3::one, Vector3::zero, Vector3::zero);
	};
public:
	Material materials;

private:
	ConstantBuffer<Material> cMaterial;

	ModelAnimation* animation_ = nullptr;

};
