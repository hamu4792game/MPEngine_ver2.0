#pragma once
#include "Graphics/Sprite/Sprite.h"
#include <array>

class Pause {
public:
	Pause();
	~Pause() = default;

public:
	enum Menu {
		Restart,
		End,

		kMaxNum
	};

	void Initialize();

	Menu Update(bool& flag);

private:
	// キー入力で切り替え
	void InputKey(bool& flag);

	// テクスチャとマウスの点との衝突判定をとる
	bool HitTestTexture(const Sprite& texture, const Vector2& mousePos);

	Menu nowMenu_ = Menu::Restart;

	std::shared_ptr<Sprite> backGround_;
	std::array<std::shared_ptr<Sprite>, Menu::kMaxNum> menus_;

};