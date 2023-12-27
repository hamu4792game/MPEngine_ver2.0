#pragma once
#include "MPEngine/Framework/SceneFrame.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/Player/Player.h"
#include "Game/FollowCamera/FollowCamera.h"
#include "Game/Stage/Stage.h"

class BattleScene : public SceneFrame {
public:
	BattleScene() = default;
	~BattleScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void DrawImGui();
private:
	WorldTransform cameraTrans_;
	std::unique_ptr<Player> player_;
	std::shared_ptr<FollowCamera> followCamera_;
	std::unique_ptr<Stage> stage_;

};
