#pragma once
#include "MPEngine/Graphics/Sprite/Sprite.h"
#include "MPEngine/Graphics/Model/Model.h"
#include "Framework/GameFrame.h"

class GameScene : public GameFrame {
public:
	GameScene() = default;
	~GameScene() override = default;

	void Initialize() override;
	void Update() override;

private:
	std::shared_ptr<Sprite> sprite;
	std::shared_ptr<Model> model;
};
