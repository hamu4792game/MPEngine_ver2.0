#include "PlayerManager.h"
#include <Utils/GlobalVariables/GlobalVariables.h>
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include <algorithm>
#include "Graphics/PostEffect/RadialBlur.h"

void PlayerManager::SpeedParam::AddUpdate() {
	acceleration += accelerationRate;
	acceleration = std::clamp(acceleration, kMinAcceleration, kMaxAcceleration);
}

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
	followCamera_->SetParam(Vector3(0.0f, 2.0f, -20.0f), followCamera_->GetTransform().rotation_, 0.95f);

	moveParameter_.Initialize(0.0f, 0.01f, 0.0f, 0.5f);

}

void PlayerManager::Update() {
	// 更新時の初期化処理
	oldPosition_ = transform_.GetPosition();
	behaviorFlag_.Initialize();
	moveVector_ = Vector3::zero;
	DrawImGui();

	// 入力情報の取得。フラグ管理
	KeyInput();

	// std::nullopt以外の時通る
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		float angle = 0.0f;
		// 振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			// カメラからターゲットへの向きベクトルを算出
			if (targetTransform_) {
				// 角度を計算
				Vector3 direction = targetTransform_->GetPosition() - transform_.GetPosition();
				float dot = direction.Normalize() * Vector3::front;
				float angleY = std::acosf(dot);
				// 左側にターゲットがあるのであれば反転
				if (direction.x < 0.0f) {
					angleY = -angleY;
				}

				//direction = targetTransform_->GetPosition() - followCamera_->GetPostposition();
				dot = direction.Normalize() * Vector3::up;
				float angleX = std::acosf(dot);
				if (direction.y > 0.0f) {
					angleX = -angleX;
				}
				
				followCamera_->SetParam(Vector3(0.0f, 0.0f, -10.0f), Vector3(followCamera_->GetTransform().rotation_.x, angleY, followCamera_->GetTransform().rotation_.z), 0.05f);
			}
			oldMoveVector = Vector3::zero;
			moveParameter_.AccelInit();
			
			break;
		case Behavior::kWebSwing:
			break;
		case Behavior::kDash:
			break;
		case Behavior::kWireMove:
			masterSpeed_ = 1.0f;
			frameCount_.count = 0.0f;
			followCamera_->SetParam(Vector3(0.0f, 2.0f, -20.0f), followCamera_->GetTransform().rotation_, 0.95f);
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
			transform_.UpdateMatrix();
			wireTargetMove_->Initialize(targetTransform_->GetPosition(), transform_.GetPosition());
			behaviorRequest_ = Behavior::kWireMove;
		}
		break;
	case Behavior::kWebSwing:
		break;
	case Behavior::kDash:
		break;
	case Behavior::kWireMove:
		Vector3 result;
		bool isSwing; //= webswing_->Update(transform_.GetPosition(), result);
		isSwing = wireTargetMove_->Update(transform_.GetPosition(), result);
		if (isSwing) {
			inputParam_.isJump = true;
			fallParam_.JumpInitialize();
			FalledProcess();
			masterSpeed_ = 0.25f;
			behaviorRequest_ = Behavior::kRoot;
		}
		moveVector_ += result;

		break;

	}

	// 事後処理----------------------------------------

	// 最終的な移動ベクトルをplayerに加算 現状はmoveManagerから取得しているが、のちに攻撃が実装されればいろいろ変わるかも
	
	// 壁に横向きで当たっている場合
	if (hittingObjectNormal_ != Vector3::zero) {
		Vector3 lNormalVec = hittingObjectNormal_.Normalize();
		// 角度の計算
		float angle = std::acosf(Vector3::up * lNormalVec);
		// 回転軸の計算
		Vector3 rotAxis = Cross(Vector3::up, lNormalVec).Normalize();
		Quaternion qur = Quaternion::MakeRotateAxisAngleQuaternion(rotAxis, angle);
		// 移動ベクトルを回転
		moveVector_ = qur * moveVector_;
		hittingObjectNormal_ = Vector3::zero;
	}

	oldMoveVector = moveVector_;
	transform_.translation_ += moveVector_;

	LimitMoving();
	followCamera_->CameraMove();
	TransformUpdate();
}

WorldTransform PlayerManager::PostUpdate() {
	//アニメーションの更新
	animation_->Update(behaviorFlag_);
	postEffectNum_ = PostEffectNum::None;
	bool finishFrag = false;
	if (masterSpeed_ != 1.0f) {
		frameCount_.count++;
		postEffectNum_ = PostEffectNum::GrayScale;
		// ターゲットがなければ即終了
		if (!targetTransform_) {
			finishFrag = true;
		}
	}
	if (frameCount_.count >= frameCount_.maxFrame * 60.0f || finishFrag || (behavior_ == Behavior::kWireMove && behaviorFlag_.isLanded)) {
		frameCount_.count = 0.0f;
		masterSpeed_ = 1.0f;
		followCamera_->SetParam(Vector3(0.0f, 2.0f, -20.0f), followCamera_->GetTransform().rotation_, 0.95f);
	}

	// followカメラの更新
	WorldTransform cameraTrans;

	// 速度の計算 (現在座標ー過去座標)/時間 今回は1.0fなので割らない
	float velocity = moveParameter_.acceleration;
	if (behavior_ == Behavior::kWireMove) {
		velocity = Length(transform_.GetPosition() - oldPosition_);
		if (velocity >= 1.0f) {
			velocity = 1.0f;
		}
	}
	else {
		velocity = 0.0f;
	}
	/// 仮で、速度が1以上ならエフェクトを
	if (velocity > 0.5f) {
		postEffectNum_ = PostEffectNum::RadialBlur;
		RadialBlur::GetInstance()->cParam_->blurWidth = -(velocity * 0.01f);
		
	}
	followCamera_->Update(velocity);
	cameraTrans = followCamera_->GetTransform();
	return cameraTrans;
}

bool PlayerManager::OnCollisionStage(const Collider& coll) {
	Vector3 pushBackVec;
	bool iscoll = collision_->OnCollision(coll, pushBackVec);
	if (iscoll) {
		// Goalと当たっていた場合
		if (coll.GetName() == "Goal") {
			// 即クリア
			return true;
		}
		// Groundと当たっていた場合
		else if (coll.GetName() == "Ground") {

			isWebSwing_ = false;

			Vector3 nPushBackVec = pushBackVec.Normalize();
			float nBigVec = pushBackVec.GetBigVector();

			// 押し戻しの値がyに大きかった場合
			if (nBigVec == std::fabsf(nPushBackVec.y)) {
				// 地面と当たっているので初期化
				if (!fallParam_.isJumpable) {
					behaviorFlag_.isLanded = true;
				}
				fallParam_.JumpInitialize();
			}
			// 横向きに当たったら
			else if (nBigVec == nPushBackVec.x || nBigVec == nPushBackVec.z) {
				// 地面と当たっているので初期化
				if (!fallParam_.isJumpable) {
					behaviorFlag_.isLanded = true;
				}
				fallParam_.JumpInitialize();
				hittingObjectNormal_ = nPushBackVec;
			}
			
			transform_.translation_ += pushBackVec;
			TransformUpdate();
			if (behavior_ == Behavior::kWireMove) {
				behaviorRequest_ = Behavior::kRoot;
			}
		}

	}
	return false;
}

WorldTransform PlayerManager::OnCollisionCameraToStage(const Collider& coll) {
	followCamera_->OnCollision(coll);
	return followCamera_->GetTransform();
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

	// ダッシュの有無
	if (inputParam_.isDashMove) {
		moveParameter_.kMaxAcceleration = 1.0f;
		moveParameter_.accelerationRate = 0.08f;
	}
	else {
		moveParameter_.kMaxAcceleration = 0.5f;
		moveParameter_.accelerationRate = 0.05f;
	}

	// 移動入力している場合
	if (inputParam_.move != Vector3::zero) {

		moveParameter_.AddUpdate();

		// 移動ベクトルに速度を足す 既に入力の状態で正規化されているのでそのまま
		float speed = moveParameter_.acceleration;
		Vector3 move = inputParam_.move * speed * masterSpeed_;

		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, Camera3d::GetInstance()->GetTransform().rotation_);
		move.y = 0.0f;
		moveVector_ += move;
		transform_.rotation_.y = FindAngle(move, Vector3(0.0f, 0.0f, 1.0f));

		// 移動しているので
		behaviorFlag_.isMoved = true;
		oldMoveVector = move;
	}
	else {
		moveParameter_.AccelInit();
		behaviorFlag_.isWaiting = true;
		oldMoveVector = inputParam_.move;
	}

	// ダッシュ中の速度が最大の時にジャンプをした場合
	if (moveParameter_.acceleration >= moveParameter_.kMaxAcceleration && inputParam_.isJump && fallParam_.isJumpable && !fallParam_.isFalled && inputParam_.isDashMove) {
		//followCamera_->StopFollow(10.0f);
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
	fallParam_.fall.acceleration -= fallParam_.fall.accelerationRate * masterSpeed_;
	// 速度制限
	//fallParam_.fall.acceleration = std::max(fallParam_.fall.acceleration, fallParam_.fall.kMaxAcceleration);
	moveVector_.y += fallParam_.fall.acceleration * masterSpeed_;
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
	if (input->GetKey()->PressKey(DIK_LSHIFT)) {
		inputParam_.isDashMove = true;
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


	if (input->GetKey()->PressKey(DIK_N)) {
		inputParam_.isSwingMove = true;
	}

#pragma endregion

	if (input->GetKey()->PressKey(DIK_R)) {
		Initialize(respawnpoint_);
	}

}

void PlayerManager::BehaviorRootUpdate() {
	InputMove();

	// webswingが押された場合
	if (inputParam_.isSwingMove && !isWebSwing_) {
		if (targetTransform_) {
			webswing_->Initialize(targetTransform_->GetPosition(), transform_.GetPosition(), moveVector_);
			isWebSwing_ = true;
		}
	}
	else if(!inputParam_.isSwingMove) {
		isWebSwing_ = false;
	}

	// ウェブスイング中か
	if (isWebSwing_) {
		moveVector_ += webswing_->Update(transform_.GetPosition());
	}
	else {
		FalledProcess();
	}

}
