#include "BattleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include "externals/imgui/imgui.h"

void BattleScene::Initialize() {
	player_ = std::make_unique<Player>();
	player_->Initialize();
	cameraTrans_.scale_ = Vector3::one;
	cameraTrans_.translation_ = Vector3(0.0f, 0.0f, -10.0f);
	
	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
}

void BattleScene::Finalize() {
	
}

void BattleScene::Update() {
	DrawImGui();
	stage_->Update();
	std::list<std::shared_ptr<Target>> listData(stage_->GetTargets());

	lockOn_->Update(listData);
	player_->SetTargetTrans(lockOn_->GetTargetTrans());


	player_->Update();
	enemy_->Update();
	player_->OnCollisionStage(enemy_->GetCollision());
	for (auto coll : stage_->GetCollision()) {
		player_->OnCollisionStage(coll);
	}

	Camera3d::GetInstance()->SetTransform(player_->PostUpdate());

}

void BattleScene::DrawImGui() {
	
}
