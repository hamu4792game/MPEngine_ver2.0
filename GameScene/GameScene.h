#pragma once
#include "MPEngine/Graphics/Sprite/Sprite.h"

class GameScene {
public:
	GameScene() = default;
	~GameScene() = default;

	void Initialize();
	void Draw();

private:
	std::shared_ptr<Sprite> sprite;
	std::shared_ptr<Sprite> tex;
};
