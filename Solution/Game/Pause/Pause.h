#pragma once
#include "Graphics/Sprite/Sprite.h"

class Pause {
public:
	Pause() = default;
	~Pause() = default;

public:
	enum Menu {
		Restart,
		End,

		kMaxNum
	};

	Menu Update(bool& flag);

private:
	void InputKey(bool& flag);

	Menu nowMenu_;

};