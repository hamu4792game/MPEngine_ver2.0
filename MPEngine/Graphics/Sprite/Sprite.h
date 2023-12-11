#pragma once
#include <list>
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

class Sprite {
	friend class SpriteRenderer;
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

public:
	// Textureのセット
	void SetTexture(const std::shared_ptr<Texture>& texture) { texture_ = texture; }
	void SetAnchorPoint(AnchorPoint anchor) { anchor_ = anchor; }
	void SetBlend(BlendMode blend) { blendType_ = blend; }

private:
	static std::list<Sprite*> spriteLists_;
	std::shared_ptr<Texture> texture_;
	BlendMode blendType_ = BlendMode::Normal;
	AnchorPoint anchor_ = AnchorPoint::Center;
};
