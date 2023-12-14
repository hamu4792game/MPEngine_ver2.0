#pragma once
#include <list>
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

#include "MPEngine/Math/Vector2.h"
#include "MPEngine/Base/ConstantBuffer.h"
#include "MPEngine/Math/Matrix4x4.h"

class Sprite {
	friend class SpriteRender;
public:
	Sprite();
	~Sprite();

	enum class AnchorPoint {
		Center,		// 中央
		LeftTop,	// 左上
		RightTop,	// 右上
		LeftBottom,	// 左下
		RightBottom,// 右下
	};

public: // セッター
	// Textureのセット
	void SetTexture(const std::shared_ptr<Texture>& texture) { texture_ = texture; }
	void SetBlend(BlendMode blend) { blendType_ = blend; }
	void SetAnchorPoint(AnchorPoint anchor);
	void SetIsActive(const bool& active) { isActive_ = active; }

public: // ゲッター
	uint32_t GetLayerNum() const { return layerNumber_; }

private:
	void CreateVertexResource(AnchorPoint anchor = AnchorPoint::Center);

private:
	static std::list<Sprite*> spriteLists_;
	std::shared_ptr<Texture> texture_;
	BlendMode blendType_ = BlendMode::Normal;
	AnchorPoint anchor_ = AnchorPoint::Center;
	uint32_t layerNumber_ = 0u; // 描画順。0から描画される
	bool isActive_ = true; // 描画するか否か

	Vector2 scale_;
	float rotate_;
	Vector2 translate_;

	Vector2 texcoordBase_ = Vector2::zero; // UVの基盤座標。基本左上の(0,0)
	Vector2 texcoordSize_ = Vector2::zero; // UVサイズ
	Vector4 color_;

	// 描画必要情報
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;

	struct TransformationMatrix {
		Matrix4x4 wvp;
	};
	ConstantBuffer<TransformationMatrix> cMat;
	struct Material	{
		Vector4 color;
	};
	ConstantBuffer<Material> cMaterial;

};
