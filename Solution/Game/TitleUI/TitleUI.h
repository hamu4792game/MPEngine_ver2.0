#pragma once
#include "Graphics/Sprite/Sprite.h"
#include <memory>

class TitleUI {
public:
	TitleUI() = default;
	~TitleUI() = default;

	void Initialize();
	void Update();
	void DrawImGui();

private:
	std::shared_ptr<Sprite> titleText_;
	std::shared_ptr<Sprite> pushText_;
	Vector4 color_;
	bool isInOut_ = false;

};