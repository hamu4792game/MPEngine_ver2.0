#pragma once
#pragma once
#include <list>
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

#include "MPEngine/Base/ConstantBuffer.h"
#include "MPEngine/Math/MathUtl.h"

class SkyBox {
	friend class SkyBoxRender;
public:
	SkyBox();
	~SkyBox();

public: // セッター
	// Textureのセット
	void SetTexture(Texture* texture);
	void SetIsActive(const bool& active) { isActive_ = active; }
	void SetScale(const Vector3& scale) { scale_ = scale; }
	void SetRotate(const float& rotate) { rotate_ = rotate; }
	void SetTranslate(const Vector3& translate) { translate_ = translate; }
	void SetColor(const Vector4& color) { color_ = color; }
	void SetUVSize(const Vector2& uv) { uvScale_ = uv; }
	void SetLayerNum(const uint32_t& num) { layerNumber_ = num; }


public: // ゲッター
	uint32_t GetLayerNum() const { return layerNumber_; }

private:
	void CreateVertexResource();
	void CreateIndexResource();
	void UploadVertexData();

private:
	static std::list<SkyBox*> skyBoxLists_;
	Texture* texture_ = nullptr;
	uint32_t layerNumber_ = 0u; // 描画順。0から描画される
	bool isActive_ = true; // 描画するか否か

	Vector3 scale_;
	float rotate_ = 0.0f;
	Vector3 translate_;

public:
	Vector2 uvScale_ = Vector2::one; // UVサイズ
	float uvRotate_ = 0.0f; // UVの回転
	Vector2 uvTranslate_ = Vector2::zero; // UVの基盤座標

private:
	Vector4 color_ = Vector4::one;

	// 描画必要情報
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;

	struct TransformationMatrix {
		Matrix4x4 wvp;
	};
	ConstantBuffer<TransformationMatrix> cMat;
	struct Material {
		Vector4 color;
		Matrix4x4 uvMat;
	};
	ConstantBuffer<Material> cMaterial;

};
