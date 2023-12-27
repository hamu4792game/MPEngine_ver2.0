#include "Player.h"
#include "externals/imgui/imgui.h"
#include "Input/input.h"
#include "Utils/Camera/Camera3d.h"

void Player::Initialize() {
	auto rsManager = ResourceManager::GetInstance();
	bodyModel_ = std::make_shared<Model>();
	bodyModel_->SetModel(rsManager->FindObject3d("PlayerBody"));

	transform_.scale_ = Vector3::one;
	transform_.translation_.y = 1.0f;

}

void Player::Update() {
	DrawImGui();

	// std::nullopt以外の時通る
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			break;
		case Behavior::kAttack:
			break;
		case Behavior::kDash:
			break;
		}
		//	振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		// 通常行動
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		break;
	case Behavior::kDash:
		break;
	}

	LimitMoving();
	TransformUpdate();
}

void Player::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG
}

void Player::Move() {
	auto input = Input::GetInstance();
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

void Player::Jamp() {
	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_SPACE) && !isJamped_) {
		// 初速度を与える
		isJamped_ = true;
		acceleration_ = 1.0f;
	}

	if (isJamped_) {
		// ジャンプ更新処理
		// 重力
		const float gravity_ = 0.1f;
		// 重力を足していく
		acceleration_ -= gravity_;
	}
	transform_.translation_.y += acceleration_;

}

void Player::TransformUpdate() {
	transform_.UpdateMatrix();
	bodyModel_->transform_ = transform_;
}

void Player::LimitMoving() {
	if (transform_.translation_.y < 1.0f) {
		transform_.translation_.y = 1.0f;
		isJamped_ = false;
	}
}

void Player::BehaviorRootUpdate() {
	Move();
	Jamp();
}
