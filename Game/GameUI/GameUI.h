#pragma once
#include "Graphics/Sprite/Sprite.h"

class GameUI {
public:
	GameUI() = default;
	~GameUI() = default;

	void Initialize();
	void Update();

private:
	std::shared_ptr<Sprite> hud_;


};
