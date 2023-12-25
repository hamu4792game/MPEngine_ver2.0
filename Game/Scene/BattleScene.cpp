#include "BattleScene.h"
#include "externals/imgui/imgui.h"
#include "Utils/Camera/Camera3d.h"

void BattleScene::Initialize() {
	player_ = std::make_unique<Player>();
	player_->Initialize();
	cameraTrans_.scale_ = Vector3::one;
}

void BattleScene::Finalize() {
	
}

void BattleScene::Update() {
	DrawImGui();

	player_->Update();

	cameraTrans_.UpdateMatrix();
	Camera3d::GetInstance()->SetTransform(cameraTrans_);
}

void BattleScene::DrawImGui() {
	ImGui::DragFloat3("position", &cameraTrans_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &cameraTrans_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &cameraTrans_.scale_.x, 0.1f);
}
