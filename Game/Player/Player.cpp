#include "Player.h"
#include "externals/imgui/imgui.h"
#include "Input/input.h"
#include "Utils/Camera/Camera3d.h"

void Player::Initialize() {
	auto rsManager = ResourceManager::GetInstance();
	model_ = std::make_shared<Model>();
	model_->SetModel(rsManager->FindObject3d("Box"));

	transform_.scale_ = Vector3::one;

}

void Player::Update() {
	DrawImGui();

	Move();

	transform_.UpdateMatrix();
	model_->transform_ = transform_;
}

void Player::DrawImGui() {
	ImGui::Begin("Player");
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::End();
}

void Player::Move() {
	auto input = Input::GetInstance();
	if (input->GetKey()->PressKey(DIK_SPACE)) {
		model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ABCD"));
	}
	Vector3 move;

	const float speed = 0.2f;
	if (input->GetKey()->PressKey(DIK_W)) {
		move.z += speed;
	}
	if (input->GetKey()->PressKey(DIK_S)) {
		move.z -= speed;
	}
	if (input->GetKey()->PressKey(DIK_A)) {
		move.x -= speed;
	}
	if (input->GetKey()->PressKey(DIK_D)) {
		move.x += speed;
	}

	
	if (move != Vector3::zero) {
		move = Normalize(move);
		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, Camera3d::GetInstance()->GetTransform().rotation_);
		move.y = 0.0f;
		transform_.translation_ += move;
	}

}
