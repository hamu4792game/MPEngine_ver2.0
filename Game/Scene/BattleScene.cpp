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
}

void BattleScene::Finalize() {
	
}

void BattleScene::Update() {
	DrawImGui();
	stage_->Update();

	player_->Update();
	followCamera_->Update();
	cameraTrans_ = followCamera_->GetTransform();
	cameraTrans_.UpdateMatrix();
	Camera3d::GetInstance()->SetTransform(cameraTrans_);

}

void BattleScene::DrawImGui() {
	
}
