#pragma once
#include "MPEngine/Framework/SceneFrame.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/Player/Player.h"
#include "Game/Stage/Stage.h"
#include "Game/Target/Target.h"
#include <vector>
#include "Game/LockOn/LockOn.h"
#include "Game/Enemy/Enemy.h"
#include "Game/GameUI/GameUI.h"

#include "Graphics/Line/Line.h"

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
	std::unique_ptr<Stage> stage_;
	//std::vector<std::shared_ptr<Target>> targets_;
	std::unique_ptr<LockOn> lockOn_;
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<GameUI> gameUI_;
	std::unique_ptr<Line> line_;
};
