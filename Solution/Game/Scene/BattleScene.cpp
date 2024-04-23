#include "BattleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "ResourceManager/ResourceManager.h"
#include "Input/Audio.h"

void BattleScene::Initialize() {
	player_ = std::make_unique<Player>();
	player_->Initialize();
	cameraTrans_.scale_ = Vector3::one;
	cameraTrans_.translation_ = Vector3(0.0f, 0.0f, -10.0f);
	
	stage_ = std::make_unique<Stage>();
	stage_->Initialize("Stage");

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	gameUI_ = std::make_unique<GameUI>();
	gameUI_->Initialize();

	// 音
	auto rs = ResourceManager::GetInstance();
	Audio* bgm = rs->FindAudio("Battle");
	bgm->SoundPlayWave(true);

}

void BattleScene::Finalize() {
	
}

void BattleScene::Update() {
	DrawImGui();
	stage_->Update();
	std::list<std::shared_ptr<Target>> listData(stage_->GetTargets());

	lockOn_->Update(listData);
	auto handle = lockOn_->GetTargetTrans();
	//player_->SetTargetTrans(&handle);


	player_->Update();
	enemy_->Update();
	//player_->OnCollisionStage(enemy_->GetCollision());
	//if (player_->OnCollision(enemy_->GetCollision())) {
	//	endRequest_ = true;
	//}

	for (auto coll : stage_->GetCollision()) {
		player_->OnCollisionStage(coll);
	}
	gameUI_->Update();

	Camera3d::GetInstance()->SetTransform(player_->PostUpdate());

	if (Input::GetInstance()->GetKey()->PressKey(DIK_P)) {
		endRequest_ = true;
	}

}

void BattleScene::DrawImGui() {
#ifdef _DEBUG

#endif // _DEBUG
}
