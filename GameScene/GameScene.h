#pragma once
#include "Framework/GameFrame.h"
#include "Game/Scene/BattleScene.h"
#include "Game/BoxSky/BoxSky.h"
#include <memory>

class GameScene : public GameFrame {
public:
	GameScene() = default;
	~GameScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;

private:
	BattleScene* battleScene = nullptr;
	std::unique_ptr<BoxSky> boxSky_;

};
