#include "BattleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"

void BattleScene::Initialize() {
	player_ = std::make_unique<Player>();
	player_->Initialize();
	cameraTrans_.scale_ = Vector3::one;
	cameraTrans_.translation_ = Vector3(0.0f, 0.0f, -10.0f);
	followCamera_ = std::make_shared<FollowCamera>();

	followCamera_->SetTarget(&player_->GetTransform());
	followCamera_->Initialize();

	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	const uint32_t kMaxTargetNum = 3u;
	Vector3 targetIniPos[kMaxTargetNum]{
		Vector3(0.0f,10.0f,0.0f),
		Vector3(40.0f,15.0f,-20.0f),
		Vector3(-20.0f,15.0f,20.0f),
	};
	for (uint32_t index = 0; index < kMaxTargetNum; index++) {
		targets_.emplace_back(std::make_shared<Target>())->Initialize(targetIniPos[index]);
	}

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
}

void BattleScene::Finalize() {
	
}

void BattleScene::Update() {
	DrawImGui();
	stage_->Update();
	std::list<std::shared_ptr<Target>> listData(targets_.begin(), targets_.end());
	lockOn_->Update(listData);
	player_->SetTargetTrans(lockOn_->GetTargetTrans());


	player_->Update();
	enemy_->Update();
	player_->OnCollision(enemy_->GetCollision());



	followCamera_->Update();
	cameraTrans_ = followCamera_->GetTransform();
	cameraTrans_.UpdateMatrix();
	Camera3d::GetInstance()->SetTransform(cameraTrans_);

}

void BattleScene::DrawImGui() {
	
}
