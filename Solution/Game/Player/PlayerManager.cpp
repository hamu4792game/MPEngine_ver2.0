#include "PlayerManager.h"
#include <Utils/GlobalVariables/GlobalVariables.h>
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include <algorithm>

PlayerManager::PlayerManager() {
	animation_ = std::make_unique<PlayerAnimation>(&transform_);
	collision_ = std::make_unique<Collider>();
	followCamera_ = std::make_shared<FollowCamera>();
	webswing_ = std::make_unique<WebSwing>();
	wireTargetMove_ = std::make_unique<WireTargetMove>();
}

void PlayerManager::Initialize(const WorldTransform& respawnpoint) {
	auto global = GlobalVariables::GetInstance();
	//global->LoadFile(itemName_);
	respawnpoint_ = respawnpoint;
	
	transform_ = respawnpoint;
	transform_.UpdateMatrix();
	animation_->Initialize();

	collTrans_.parent_ = &transform_;
	collTrans_.scale_ = Vector3(1.0f, 2.4f, 1.0f);
	collision_->Initialize(&collTrans_, Collider::Type::Box);
	
	fallParam_.JumpInitialize();

	followCamera_->SetTarget(&transform_);
	followCamera_->Initialize();
}

void PlayerManager::Update() {
	// 更新時の初期化処理
	behaviorFlag_.Initialize();
	moveVector_ = Vector3::zero;
	DrawImGui();

	// 入力情報の取得。フラグ管理
	KeyInput();

	// std::nullopt以外の時通る
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			followCamera_->SetTarget(&transform_);
			//followCamera_->SetParam(Vector3(0.0f, 2.0f, -10.0f), Vector3(AngleToRadian(5.0f), transform_.rotation_.y, followCamera_->GetTransform().rotation_.z), 0.05f);
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
		if (inputParam_.isWireMove && targetTransform_) {
			//webswing_->SetWeb(targetTransform_->GetPosition(), transform_.GetPosition());
			transform_.UpdateMatrix();
			wireTargetMove_->Initialize(targetTransform_->GetPosition(), transform_.GetPosition());
			behaviorRequest_ = Behavior::kSwing;
		}
		break;
	case Behavior::kAttack:
		break;
	case Behavior::kDash:
		break;
	case Behavior::kSwing:
		Vector3 result;
		bool isSwing; //= webswing_->Update(transform_.GetPosition(), result);
		isSwing = wireTargetMove_->Update(transform_.GetPosition(), result);
		if (isSwing) {
			inputParam_.isJump = true;
			fallParam_.JumpInitialize();
			FalledProcess();
			behaviorRequest_ = Behavior::kRoot;
		}
		moveVector_ += result;

		break;

	}

	// 事後処理----------------------------------------

	// 最終的な移動ベクトルをplayerに加算 現状はmoveManagerから取得しているが、のちに攻撃が実装されればいろいろ変わるかも
	
	transform_.translation_ += moveVector_;

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

void PlayerManager::OnCollisionStage(const Collider& coll) {
	// ローカル変数にrつけてるだけ
	Vector3 pushBackVec;
	bool iscoll = collision_->OnCollision(coll, pushBackVec);
	// 床との衝突判定
	if (iscoll) {
		// 押し戻しが上の処理だった場合
 		if (/*pushBackVec.x < pushBackVec.y && pushBackVec.z < pushBackVec.y*/
			pushBackVec.Normalize() == Vector3::up) {
			// 地面と当たっているので初期化
			if (!fallParam_.isJumpable) {
				behaviorFlag_.isLanded = true;
			}
			fallParam_.JumpInitialize();
		}
		// 横向きに当たったら
		else if (pushBackVec.Normalize() == Vector3::left || pushBackVec.Normalize() == Vector3::right) {

		}

		transform_.translation_ += pushBackVec;
		TransformUpdate();
		if (behavior_ == Behavior::kSwing) {
			behaviorRequest_ = Behavior::kRoot;
		}
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

void PlayerManager::InputMove() {
	if (inputParam_.move != Vector3::zero) {
		// 移動ベクトルに速度を足す 既に入力の状態で正規化されているのでそのまま
		const float speed = 0.5f;
		Vector3 move = inputParam_.move * speed;

		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, Camera3d::GetInstance()->GetTransform().rotation_);
		move.y = 0.0f;
		moveVector_ += move;
		transform_.rotation_.y = FindAngle(move, Vector3(0.0f, 0.0f, 1.0f));

		// 移動しているので
		behaviorFlag_.isMoved = true;
	}
	else {
		behaviorFlag_.isWaiting = true;
	}
}

void PlayerManager::FalledProcess() {

	if (inputParam_.isJump && fallParam_.isJumpable && !fallParam_.isFalled) {
		// 初速度を与える
		fallParam_.isJumpable = false;
		fallParam_.isFalled = true;
		fallParam_.fall.Initialize(1.0f, 0.05f, 0.0f, -3.0f);
		fallParam_.fall.acceleration = 1.0f;
	}
	// 落下更新処理
	// 重力を足していく
	fallParam_.fall.acceleration -= fallParam_.fall.accelerationRate;
	// 速度制限
	//fallParam_.fall.acceleration = std::max(fallParam_.fall.acceleration, fallParam_.fall.kMaxAcceleration);
	moveVector_.y += fallParam_.fall.acceleration;
	if (fallParam_.isFalled) {
		if (fallParam_.fall.acceleration < 0.0f) {
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
	collTrans_.UpdateMatrix();
	collision_->Update();
}

void PlayerManager::LimitMoving() {
	if (transform_.translation_.y < -10.0f) {
		Initialize(respawnpoint_);
	}
	//transform_.translation_.y = std::clamp(transform_.translation_.y, 15.0f, 10000.0f);
}

void PlayerManager::KeyInput() {
	auto input = Input::GetInstance();
	inputParam_.Initialize();
#pragma region Jump
	// ジャンプ用入力
	if (input->GetKey()->TriggerKey(DIK_SPACE)) {
		inputParam_.isJump = true;
	}
	if (input->GetPad()->GetPadConnect()) {
		if (input->GetPad()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			inputParam_.isJump = true;
		}
	}
#pragma endregion

#pragma region Move
	// 通常移動入力
	// 単位ベクトルで対応
	const float speed = 1.0f;
	if (input->GetKey()->PressKey(DIK_W)) {
		inputParam_.move.z += speed;
	}
	if (input->GetKey()->PressKey(DIK_S)) {
		inputParam_.move.z -= speed;
	}
	if (input->GetKey()->PressKey(DIK_A)) {
		inputParam_.move.x -= speed;
	}
	if (input->GetKey()->PressKey(DIK_D)) {
		inputParam_.move.x += speed;
	}

	if (input->GetPad()->GetPadConnect()) {
		Vector2 pmove(0.0f, 0.0f);
		pmove = input->GetPad()->GetPadLStick();
		//	移動があれば代入する
		if (pmove.x != 0.0f || pmove.y != 0.0f)
		{
			inputParam_.move.x = pmove.x;
			inputParam_.move.z = pmove.y;
		}
	}
	inputParam_.move = inputParam_.move.Normalize();

#pragma endregion

#pragma region WireMove
	
	if (input->GetKey()->TriggerKey(DIK_M)) {
		inputParam_.isWireMove = true;
	}
	if (input->GetPad()->GetPadConnect()) {
		if (input->GetPad()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			inputParam_.isWireMove = true;
		}
	}

#pragma endregion

	if (input->GetKey()->TriggerKey(DIK_R)) {
		Initialize(respawnpoint_);
	}

}

void PlayerManager::BehaviorRootUpdate() {
	InputMove();
	FalledProcess();
}
