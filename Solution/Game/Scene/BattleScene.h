#pragma once
#include "MPEngine/Framework/SceneFrame.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/Player/PlayerManager.h"
#include "Game/Stage/Stage.h"
#include "Game/Target/Target.h"
#include <vector>
#include "Game/LockOn/LockOn.h"
#include "Game/Enemy/Enemy.h"
#include "Game/GameUI/GameUI.h"
#include "MPEngine/Graphics/SkyBox/SkyBox.h"
#include "Game/Pause/Pause.h"
#include "Game/LockOn/PointOfGazeSearch.h"

// ゲームのメインシーン
class BattleScene : public SceneFrame {
public:
	BattleScene() = default;
	~BattleScene() override = default;

	void Initialize() override;
	void Finalize() override;
	void Update() override;
	void SecondUpdate() override; // アニメーション等、衝突判定後に更新したいやつ
	void DrawImGui();

private:
	void PauseMenu();

private:
	std::unique_ptr<PlayerManager> player_;
	std::unique_ptr<Stage> stage_;
	std::unique_ptr<LockOn> lockOn_;
	std::unique_ptr<PointOfGazeSearch> pointOfGazeSearch_;
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<GameUI> gameUI_;

	std::unique_ptr<SkyBox> skybox_;


	bool isPause_ = false;
	std::unique_ptr<Pause> pause_;
	WorldTransform cameraTransform_;

	// チュートリアル用のフラグ false = もう通らない
	bool isTutorial_ = true;
};
