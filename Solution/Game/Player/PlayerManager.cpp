#include "PlayerManager.h"
#include <Utils/GlobalVariables/GlobalVariables.h>
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include <algorithm>

PlayerManager::PlayerManager() {
	animation_ = std::make_unique<PlayerAnimation>(&transform_);
	collision_ = std::make_shared<AABB>();
	followCamera_ = std::make_shared<FollowCamera>();
	webswing_ = std::make_unique<WebSwing>();
}

void PlayerManager::Initialize() {
	auto global = GlobalVariables::GetInstance();
	//global->LoadFile(itemName_);
	
	transform_.scale_ = Vector3::one;
	transform_.translation_ = Vector3(120.0f, 22.0f, -100.0f);
	//transform_.translation_ = Vector3(0.0f, 22.0f, 0.0f);
	transform_.UpdateMatrix();
	animation_->Initialize();
	collision_->size = Vector3(1.0f, 2.4f, 1.0f);
	fallParam_.Initialize();

	followCamera_->SetTarget(&transform_);
	followCamera_->Initialize();
}

void PlayerManager::Update() {
	behaviorFlag_.Initialize();
	DrawImGui();

	// std::nullopt以外の時通る
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			followCamera_->SetTarget(&transform_);
			followCamera_->SetParam(Vector3(0.0f, 2.0f, -10.0f), Vector3(AngleToRadian(5.0f), transform_.rotation_.y, followCamera_->GetTransform().rotation_.z), 0.05f);
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
		if (Input::GetInstance()->GetKey()->TriggerKey(DIK_M) && targetTransform_) {
			webswing_->SetWeb(targetTransform_->GetPosition(), transform_.GetPosition());
			behaviorRequest_ = Behavior::kSwing;
		}
		break;
	case Behavior::kAttack:
		break;
	case Behavior::kDash:
		break;
	case Behavior::kSwing:
		Vector3 result;
		bool isSwing = webswing_->Update(transform_.GetPosition(), result);
		if (isSwing) {
			behaviorRequest_ = Behavior::kRoot;
		}
		transform_.translation_ += result;
		Move();

		break;

	}

	LimitMoving();
	followCamera_->CameraMove();
	TransformUpdate();
}

WorldTransform PlayerManager::PostUpdate() {
	//アニメーションの更新
	animation_->Update(behaviorFlag_);

	// followカメラの更新
	WorldTransform cameraTrans;
	followCamera_->Update();
	cameraTrans = followCamera_->GetTransform();
	return cameraTrans;
}

void PlayerManager::OnCollisionStage(const AABB* aabb) {
	// ローカル変数にrつけてるだけ
	bool iscoll = collision_->IsCollision(aabb);
	// 床との衝突判定
	if (iscoll) {
		Vector3 extrusionVector;
		// 地面と当たっているので初期化
		if (!fallParam_.isJumpable_) {
			behaviorFlag_.isLanded = true;
		}
		fallParam_.Initialize();
		// size同士
		Vector3 rScale = collision_->boxModel_.scale_ + aabb->boxModel_.scale_;
		// 距離 playerからboxの距離
		Vector3 rPosA = collision_->boxModel_.GetPosition();
		Vector3 rPosB = aabb->boxModel_.GetPosition();
		Vector3 rDistance = rPosA - rPosB;
		// 距離がsizeよりちいさかったらめり込んでいる 当たっている時点で通るがおまじない
		// y軸
		if (std::fabs(rScale.y) > std::fabs(rDistance.y)) {
			// size - 距離の差分を求めて指定の向きに足す
			float dis = std::fabs(rScale.y) - std::fabs(rDistance.y);
			// playerがboxよりも下にあったら
			if (rPosA.y <= rPosB.y) {
				extrusionVector.y -= dis;
			}
			// playerがboxよりも上にあったら
			else if (rPosA.y > rPosB.y) {
				extrusionVector.y += dis;
			}
		}

		transform_.translation_ += extrusionVector;
		TransformUpdate();
	}
}

void PlayerManager::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin(itemName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Adjustment")) {
			if (ImGui::TreeNode("Transform")) {
				ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
				ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
				ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
				ImGui::TreePop();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();

#endif // _DEBUG
}

void PlayerManager::Move() {
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

	if (input->GetPad()->GetPadConnect()) {
		Vector2 pMove(0.0f, 0.0f);
		pMove = input->GetPad()->GetPadLStick();
		//	移動があれば代入する
		if (pMove.x != 0.0f || pMove.y != 0.0f)
		{
			move.x = pMove.x;
			move.z = pMove.y;
		}
	}

	if (move != Vector3::zero) {
		move = Normalize(move);
		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, Camera3d::GetInstance()->GetTransform().rotation_);
		move.y = 0.0f;
		transform_.translation_ += move;
		//partsTrans_[Parts::Body].rotation_.y = FindAngle(move, Vector3(0.0f, 0.0f, 1.0f));
		transform_.rotation_.y = FindAngle(move, Vector3(0.0f, 0.0f, 1.0f));

		//animation_->SetAnimation(PlayerAnimation::AnimationType::Run, true);
		behaviorFlag_.isMoved = true;
	}
	else {
		//animation_->SetAnimation(PlayerAnimation::AnimationType::Wait, true);
		behaviorFlag_.isWaiting = true;
	}
}

void PlayerManager::Jamp() {
	bool flag = false;
	auto input = Input::GetInstance();
	if (input->GetKey()->TriggerKey(DIK_SPACE)) {
		flag = true;
	}
	if (input->GetPad()->GetPadConnect()) {
		if (input->GetPad()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			flag = true;
		}
	}

	if (flag && fallParam_.isJumpable_ && !fallParam_.isFalled_) {
		// 初速度を与える
		fallParam_.isJumpable_ = false;
		fallParam_.acceleration_ = 1.0f;
		fallParam_.isFalled_ = true;
	}
	// 落下更新処理
	// 重力
	const float gravity_ = 0.05f;
	// 重力を足していく
	fallParam_.acceleration_ -= gravity_;
	transform_.translation_.y += fallParam_.acceleration_;
	if (fallParam_.isFalled_) {
		if (fallParam_.acceleration_ < 0.0f) {
			// 落下中
			behaviorFlag_.isFalled = true;
		}
		else {
			// 上昇中
			behaviorFlag_.isJumped = true;
		}
	}
}

void PlayerManager::TransformUpdate() {
	transform_.UpdateMatrix();
	collision_->Update(transform_);
}

void PlayerManager::LimitMoving() {
	transform_.translation_.y = std::clamp(transform_.translation_.y, 15.0f, 10000.0f);
	if (transform_.translation_.y < 15.0f) {
		transform_.translation_ = Vector3(0.0f, 22.0f, -100.0f);
	}
}

void PlayerManager::BehaviorRootUpdate() {
	Move();
	Jamp();
}
