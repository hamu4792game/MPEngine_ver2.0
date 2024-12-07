#pragma once
#include "Graphics/Sprite/Sprite.h"

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
	void InputKey(bool& flag);

	Menu nowMenu_ = Menu::Restart;

	std::shared_ptr<Sprite> backGround_;
	std::shared_ptr<Sprite> restart_;
	std::shared_ptr<Sprite> end_;

};