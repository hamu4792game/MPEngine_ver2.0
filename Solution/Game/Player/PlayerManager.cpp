#include "PlayerManager.h"
#include "Utils/GlobalVariables/GlobalVariables.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include <algorithm>
#include "Graphics/PostEffect/RadialBlur.h"

PlayerManager::PlayerManager() {
	animation_ = std::make_unique<PlayerAnimation>(&transform_);
	collision_ = std::make_unique<Collider>();
	followCamera_ = std::make_shared<FollowCamera>();
	webswing_ = std::make_unique<WebSwing>();
	moveCom_ = std::make_unique<MoveCommand>(itemName_);
}

void PlayerManager::Initialize(const WorldTransform& respawnpoint) {
	MoveParam moveparam;
	SetGlobalVariables(moveparam);
	moveCom_->Initialize(moveparam, &masterSpeed_);
	debugMoveParam_ = moveparam;

	respawnpoint_ = respawnpoint;
	
	transform_ = respawnpoint;
	transform_.isQuaternion_ = true;
	transform_.UpdateMatrix();
	animation_->Initialize();

	collTrans_.parent_ = &transform_;
	collTrans_.scale_ = Vector3(1.0f, 2.4f, 1.0f);
	collision_->Initialize(&collTrans_, Collider::Type::Box);
	
	fallParam_.fall.Initialize(moveparam.fallParam.acceleration, moveparam.fallParam.accelerationRate, moveparam.fallParam.kMinAcceleration, moveparam.fallParam.kMaxAcceleration);
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
				
				//followCamera_->SetParam(Vector3(0.0f, 0.0f, -10.0f), Vector3(followCamera_->GetTransform().rotation_.x, angleY, followCamera_->GetTransform().rotation_.z), 0.05f);
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
			moveCom_->ExWireTargetMove(targetTransform_->GetPosition(), transform_.GetPosition());
			behaviorRequest_ = Behavior::kWireMove;
		}
		break;
	case Behavior::kWireMove:
		Vector3 result;
		bool isSwing; //= webswing_->Update(transform_.GetPosition(), result);
		isSwing = moveCom_->UpWireTargetMove(transform_.GetPosition(), result);
		if (isSwing) {
			inputParam_.isJump = true;
			fallParam_.JumpInitialize();
			masterSpeed_ = 0.25f;
			behaviorRequest_ = Behavior::kRoot;
		}
		moveVector_ += result;

		break;

	}

	// 事後処理----------------------------------------

	// 最終的な移動ベクトルをplayerに加算 現状はmoveManagerから取得しているが、のちに攻撃が実装されればいろいろ変わるかも
	
	// 壁に横向きで当たっている場合
	Quaternion qur;
	if (hittingObjectNormal_ != Vector3::zero) {
		Vector3 lNormalVec = hittingObjectNormal_.Normalize();
		// 角度の計算
		float angle = std::acosf(Vector3::up * lNormalVec);
		// 回転軸の計算
		Vector3 rotAxis = Cross(Vector3::up, lNormalVec).Normalize();
		qur = Quaternion::MakeRotateAxisAngleQuaternion(rotAxis, angle);
		// 移動ベクトルを回転
		moveVector_ = qur * moveVector_;
		hittingObjectNormal_ = Vector3::zero;
		//qur = Quaternion::MakeFromTwoVector(lNormalVec, rotAxis);
	}
	//animation_->SetQuaternion(qur);

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
		if (ImGui::BeginMenu("Param")) {
			if (ImGui::Button("Save")) {
				AddGlobalVariables(moveCom_->GetParameter());
			}
			moveCom_->ImGuiProc();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	ImGui::End();
#endif // _DEBUG
}


void PlayerManager::TransformUpdate() {
	transform_.UpdateMatrix();
	collTrans_.UpdateMatrix();
	collision_->Update();
}

void PlayerManager::LimitMoving() {
	transform_.translation_.y = std::clamp(transform_.translation_.y, 2.4f, 10000.0f);
	//if (transform_.translation_.y < -10.0f) {
	//	Initialize(respawnpoint_);
	//}
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
	
	if (input->GetMouse()->TriggerMouse(MouseInput::RIGHT)) {
		inputParam_.isWireMove = true;
	}

	if (input->GetKey()->TriggerKey(DIK_M)) {
		inputParam_.isWireMove = true;
	}
	if (input->GetPad()->GetPadConnect()) {
		if (input->GetPad()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			inputParam_.isWireMove = true;
		}
	}

	if (input->GetMouse()->PressMouse(MouseInput::LEFT)) {
		inputParam_.isSwingMove = true;
	}
	if (input->GetMouse()->TriggerMouse(MouseInput::LEFT)) {
		inputParam_.isPushSwing = true;
	}

	if (input->GetKey()->PressKey(DIK_N)) {
		inputParam_.isSwingMove = true;
	}
	if (input->GetKey()->TriggerKey(DIK_N)) {
		inputParam_.isPushSwing = true;
	}

#pragma endregion

	if (input->GetKey()->PressKey(DIK_R)) {
		Initialize(respawnpoint_);
	}

}

void PlayerManager::BehaviorRootUpdate() {
	WorldTransform handle;
	bool isLanded = false;
	// 地面についているなら
	if (!fallParam_.isFalled) {
		isLanded = true;
	}
	// 通常移動処理
	bool isMoving = moveCom_->UpInputMove(inputParam_.move, handle, isLanded);
	if (isMoving) {
		transform_.rotationQuat_ = handle.rotationQuat_;
		moveVector_ += handle.translation_;
		// 地面についているなら
		if (isLanded) {
			behaviorFlag_.isMoved = true;
		}
	}
	else {
		behaviorFlag_.isWaiting = true;
	}

	// ジャンプを押した場合
	if (inputParam_.isJump) {
		moveCom_->ExJump(fallParam_);
	}


	// webswingが押された場合
	if (inputParam_.isPushSwing && !isWebSwing_) {
		isWebSwing_ = true;
		moveCom_->ExWebSwing(transform_, 2.0f);
	}
	else if (!inputParam_.isSwingMove) {
		isWebSwing_ = false;
	}

	// ウェブスイング中か
	if (isWebSwing_) {
		moveVector_ += moveCom_->UpWebSwing(transform_.GetPosition(), isWebSwing_);
	}
	else {
		// 重力落下処理 Y軸の座標移動
		moveVector_ += moveCom_->UpFalling(fallParam_);
		behaviorFlag_.isJumped = fallParam_.isJumped;
		behaviorFlag_.isFalled = fallParam_.isFalled;

		// ここでxz座標の移動をする
		// 落ちているなら減衰処理を
		if (fallParam_.isFalled) {
			moveVector_ += moveCom_->UpPostWebSwing();
		}
	}

}

void PlayerManager::SetGlobalVariables(MoveParam& param) {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	
	param.inputMoveParam.accelerationRate = gv->GetFloatValue(itemName_,"InputMoveParam_accelerationRate");
	param.inputMoveParam.kMinAcceleration = gv->GetFloatValue(itemName_,"InputMoveParam_kMinAcceleration");
	param.inputMoveParam.kMaxAcceleration = gv->GetFloatValue(itemName_,"InputMoveParam_kMaxAcceleration");
	param.airMoveVelocity = gv->GetFloatValue(itemName_, "InputMoveParam_AirMoveVelocity");
	
	param.wireMoveParam.accelerationRate = gv->GetFloatValue(itemName_,"WireMoveParam_accelerationRate");
	param.wireMoveParam.kMinAcceleration = gv->GetFloatValue(itemName_,"WireMoveParam_kMinAcceleration");
	param.wireMoveParam.kMaxAcceleration = gv->GetFloatValue(itemName_,"WireMoveParam_kMaxAcceleration");
	
	param.fallParam.accelerationRate = gv->GetFloatValue(itemName_,"FallParam_accelerationRate");
	param.fallParam.kMinAcceleration = gv->GetFloatValue(itemName_,"FallParam_kMinAcceleration");
	param.fallParam.kMaxAcceleration = gv->GetFloatValue(itemName_,"FallParam_kMaxAcceleration");

	param.jumpFirstVelocity = gv->GetFloatValue(itemName_, "JumpFirstVelocity");
}

void PlayerManager::AddGlobalVariables(const MoveParam& param) {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	gv->SetValue(itemName_, "InputMoveParam_accelerationRate", param.inputMoveParam.accelerationRate);
	gv->SetValue(itemName_, "InputMoveParam_kMinAcceleration", param.inputMoveParam.kMinAcceleration);
	gv->SetValue(itemName_, "InputMoveParam_kMaxAcceleration", param.inputMoveParam.kMaxAcceleration);
	gv->SetValue(itemName_, "InputMoveParam_AirMoveVelocity", param.airMoveVelocity);

	gv->SetValue(itemName_, "WireMoveParam_accelerationRate", param.wireMoveParam.accelerationRate);
	gv->SetValue(itemName_, "WireMoveParam_kMinAcceleration", param.wireMoveParam.kMinAcceleration);
	gv->SetValue(itemName_, "WireMoveParam_kMaxAcceleration", param.wireMoveParam.kMaxAcceleration);

	gv->SetValue(itemName_, "FallParam_accelerationRate", param.fallParam.accelerationRate);
	gv->SetValue(itemName_, "FallParam_kMinAcceleration", param.fallParam.kMinAcceleration);
	gv->SetValue(itemName_, "FallParam_kMaxAcceleration", param.fallParam.kMaxAcceleration);

	gv->SetValue(itemName_, "JumpFirstVelocity", param.jumpFirstVelocity);

	gv->SaveFile(itemName_);
}
