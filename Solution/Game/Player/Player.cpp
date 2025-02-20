#include "Player.h"
#include "Utils/GlobalVariables/GlobalVariables.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include <algorithm>
#include "Graphics/PostEffect/RadialBlur.h"

Player::Player() {
	animation_ = std::make_unique<PlayerAnimation>(&transform_);
	collision_ = std::make_unique<Collider>();
	downCollision_ = std::make_unique<Collider>();
	followCamera_ = std::make_shared<FollowCamera>();
	webswing_ = std::make_unique<WebSwing>();
	moveCom_ = std::make_unique<MoveCommand>();
	circleShadow_ = std::make_unique<CircleShadow>();
	wire_ = std::make_unique<Wire>();
}

void Player::Initialize(const WorldTransform& respawnpoint) {
	MoveParam moveparam;
	SetGlobalVariables(moveparam);
	moveCom_->Initialize(moveparam, &masterSpeed_);

	respawnpoint_ = respawnpoint;
	
	transform_ = respawnpoint;
	transform_.isQuaternion_ = true;
	transform_.UpdateMatrix();
	animation_->Initialize();

	collTrans_.parent_ = &transform_;
	collTrans_.scale_ = Vector3(1.0f, 2.4f, 1.0f);
	collision_->Initialize(&collTrans_, Collider::Type::Box);

	downCollision_->Initialize(&transform_, Collider::Type::Line);
	
	fallParam_.fall.Initialize(moveparam.fallParam.acceleration, moveparam.fallParam.accelerationRate, moveparam.fallParam.kMinAcceleration, moveparam.fallParam.kMaxAcceleration);
	fallParam_.JumpInitialize();

	followCamera_->SetTarget(&transform_);
	followCamera_->Initialize();
	followCamera_->SetParam(Vector3(0.0f, 2.0f, -20.0f), followCamera_->GetTransform().rotation_, 0.95f);

	moveParameter_.Initialize(0.0f, 0.01f, 0.0f, 0.5f);
	
	circleShadow_->Initialize();

}

void Player::Update() {
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
		// 振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			oldMoveVector = Vector3::zero;
			moveParameter_.AccelInit();
			wire_->FadeOutStart();
			
			break;
		case Behavior::kWebSwing:
			break;
		case Behavior::kDash:
			break;
		case Behavior::kWireMove:
			masterSpeed_ = 1.0f;
			frameCount_.count = 0.0f;
			followCamera_->SetParam(Vector3(0.0f, 2.0f, -20.0f), followCamera_->GetTransform().rotation_, 0.25f);

			// 回転の適応
			if (targetTransform_) {
				// 角度を計算
				Vector3 direction = targetTransform_->GetPosition() - transform_.GetPosition();
				float dot = direction.Normalize() * Vector3::front;
				float angleY = std::acosf(dot);
				// 左側にターゲットがあるのであれば反転
				if (direction.x < 0.0f) {
					angleY = -angleY;
				}

				transform_.rotationQuat_ = Quaternion::MakeRotateAxisAngleQuaternion(Vector3::up, angleY);

				wire_->Initialize(transform_.GetPosition(), targetTransform_->GetPosition());

			}
			behaviorFlag_.isWireJump = true;
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
		bool isSwing;
		isSwing = moveCom_->UpWireTargetMove(transform_.GetPosition(), result);
		if (isSwing) {
			fallParam_.JumpInitialize();
			moveCom_->ExJump(fallParam_);
			masterSpeed_ = 0.25f;
			behaviorRequest_ = Behavior::kRoot;
		}
		moveVector_ += result;

		break;

	}
	
	

	// 事後処理----------------------------------------

	// 最終的な移動ベクトルをplayerに加算 現状はmoveManagerから取得しているが、のちに攻撃が実装されればいろいろ変わるかも
	oldMoveVector = moveVector_;
	transform_.translation_ += moveVector_;

	LimitMoving();
	//followCamera_->CameraMove();
	TransformUpdate();
	animation_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	minDistance_ = std::numeric_limits<float>::max();
}

WorldTransform Player::PostUpdate() {
	//アニメーションの更新
	animation_->Update(behaviorFlag_);
	postEffectNum_ = PostEffectNum::None;
	bool finishFrag = false;
	if (masterSpeed_ != 1.0f) {
		frameCount_.count++;
		postEffectNum_ = PostEffectNum::GrayScale;
		// ターゲットがなければ即終了
		if (!targetTransform_) {
			//finishFrag = true;
		}
	}
	if (frameCount_.count >= frameCount_.maxFrame * 60.0f || finishFrag || (behavior_ == Behavior::kWireMove && behaviorFlag_.isLanded)) {
		frameCount_.count = 0.0f;
		masterSpeed_ = 1.0f;
		//followCamera_->SetParam(Vector3(0.0f, 2.0f, -20.0f), followCamera_->GetTransform().rotation_, 0.5f);
	}

	// followカメラの更新
	WorldTransform cameraTrans;

	// 速度の計算 (現在座標ー過去座標)/時間 今回は1.0fなので割らない
	float velocity = moveParameter_.acceleration;
	if (behavior_ == Behavior::kWireMove) {
		velocity = Length(transform_.GetPosition() - oldPosition_);
	}
	else {
		velocity = 0.0f;
	}
	/// 仮で、速度が1以上ならエフェクトを
	if (velocity > 0.5f) {
		postEffectNum_ = PostEffectNum::RadialBlur;
		// 0以上1以下になるよう制限
		float power = std::clamp(velocity, 0.0f, 1.0f);
		power = -(power * 0.01f);
		RadialBlur::GetInstance()->cParam_->blurWidth = power;
		
	}
	followCamera_->Update(velocity * 5.0f);
	cameraTrans = followCamera_->GetTransform();
	return cameraTrans;
}

bool Player::OnCollisionStage(const Collider& coll) {
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
				isWallRunning_ = true; // 壁移動しているフラグ
				// ちょっとだけめり込ませる
				//pushBackVec -= nPushBackVec * 0.01f;
			}
			
			transform_.translation_ += pushBackVec;
			TransformUpdate();
			if (behavior_ == Behavior::kWireMove) {
				behaviorRequest_ = Behavior::kRoot;
			}
		}
		else if (coll.GetName() == "CollectionObject") {
			
		}
	}
	return false;
}

bool Player::OnCollisionDownRayToStage(const Collider& coll, Vector3& hitPoint) {
	Vector3 pushBackVec = transform_.GetPosition();
	hitPoint = transform_.GetPosition();
	WorldTransform tra = WorldTransform(Vector3::one, Vector3::zero, transform_.GetPosition() + Vector3::down);
	downCollision_->Update(tra);
	bool iscoll = downCollision_->OnCollision(coll, pushBackVec);

	if (iscoll && coll.GetName() == "Ground") {
		// 衝突点を返す
		hitPoint.y = (coll.GetTransform().GetPosition().y + (MakeScale(coll.GetTransform().worldMatrix_).y)) + 0.1f;

		float distance = Length(FindVector(transform_.GetPosition(), hitPoint));
		if (minDistance_ > distance) {
			// 最小値の更新
			minDistance_ = distance;
			circleShadow_->Update(hitPoint, distance);
		}
		
	}
	return iscoll;
}

WorldTransform Player::OnCollisionCameraToStage(const Collider& coll) {
	if (followCamera_->OnCollision(coll)) {
		float dis = Length(transform_.GetPosition() - followCamera_->GetTransform().GetPosition());
		if (dis <= 40.0f) {
			dis /= 40.0f;
			animation_->SetColor(Vector4(1.0f, 1.0f, 1.0f, dis));
		}
	}
	return followCamera_->GetTransform();
}

void Player::DrawImGui() {
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


void Player::TransformUpdate() {
	transform_.UpdateMatrix();
	collTrans_.UpdateMatrix();
	collision_->Update();
}

void Player::LimitMoving() {
	transform_.translation_.y = std::clamp(transform_.translation_.y, 2.0f, 10000.0f);
	
}

void Player::KeyInput() {
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
	if (input->GetKey()->TriggerKey(DIK_LSHIFT)) {
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
	
	if (input->GetMouse()->TriggerMouse(MouseInput::LEFT)) {
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
		//inputParam_.isSwingMove = true;
	}
	if (input->GetMouse()->TriggerMouse(MouseInput::LEFT)) {
		//inputParam_.isPushSwing = true;
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

void Player::BehaviorRootUpdate() {
	WorldTransform handle;
	bool isLanded = false;
	// 地面についているなら
	if (!fallParam_.isFalled) {
		isLanded = true;
	}

	if (inputParam_.isDashMove) {
		dashCount_.count = 0.0f;
		moveCom_->ExDashStart();
		isDashing_ = true;
	}

	// 通常移動処理
	bool isMoving = moveCom_->UpInputMove(inputParam_.move, handle, isLanded, followCamera_->GetTransform());
	if (isMoving) {
		// 
		transform_.rotationQuat_ = handle.rotationQuat_;
		moveVector_ += handle.translation_;
		
		// ダッシュ用
		if (isDashing_) {
			isDashing_ = moveCom_->UpDash(moveVector_, dashCount_.count);
			// frame単位の加算
			dashCount_.count++;
		}

		// 地面についているなら
		if (isLanded) {
			behaviorFlag_.isMoved = true;
		}
		// モデルに回転軸を設定
		animation_->SetQuaternion(handle.rotationQuat_);
		temporarySaveMoveVector_ = moveVector_; // 壁から離れた時用に保存
	}
	else {
		behaviorFlag_.isWaiting = true;
	}

	Vector3 lJumpVector;

	// 壁に横向きで当たっている場合
	if (isWallRunning_) {
		// 壁法線があれば かつ 移動があれば
		if (hittingObjectNormal_ != Vector3::zero && moveVector_ != Vector3::zero) {
			
			// 壁移動
			moveVector_ = moveCom_->UpWallMove(hittingObjectNormal_, handle.rotationQuat_, moveVector_);
			
		}
		// ない場合
		else {
			if (moveVector_ == Vector3::zero) {
				inputParam_.isJump = true;
			}
			// 移動ベクトルをもとに正面方向のQuaternionを計算
			handle.rotationQuat_ = Quaternion::MakeFromTwoVector(Vector3::front, temporarySaveMoveVector_.Normalize());
			isWallRunning_ = false;
		}
		// モデルに回転軸を設定
		animation_->SetQuaternion(handle.rotationQuat_);
		// ジャンプさせる
		lJumpVector = hittingObjectNormal_;
		// 初期化
		hittingObjectNormal_ = Vector3::zero;
	}
	
	// ジャンプを押した場合
	if (inputParam_.isJump) {
		moveCom_->ExJump(fallParam_, lJumpVector);
		isWallRunning_ = false;

		// 移動ベクトルをもとに正面方向のQuaternionを計算
		handle.rotationQuat_ = Quaternion::MakeFromTwoVector(Vector3::front, temporarySaveMoveVector_.Normalize());
		// モデルに回転軸を設定
		animation_->SetQuaternion(handle.rotationQuat_);
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

		if (isWallRunning_) { return; }

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

	// wireの更新
	wire_->FadeOutUpdate();

}

void Player::SetGlobalVariables(MoveParam& param) {
	GlobalVariables* gv = GlobalVariables::GetInstance();
	
	param.inputMoveParam.SetGlobalVariables(itemName_, "InputMoveParam");
	param.airMoveVelocity = gv->GetFloatValue(itemName_, "InputMoveParam_AirMoveVelocity");
	
	param.wireMoveParam.SetGlobalVariables(itemName_, "WireMoveParam");
	
	param.fallParam.SetGlobalVariables(itemName_, "FallParam");

	param.jumpFirstVelocity = gv->GetFloatValue(itemName_, "JumpFirstVelocity");

	param.dashParam.speed.SetGlobalVariables(itemName_, "DashParam");
	param.dashParam.accelerationTime = gv->GetFloatValue(itemName_, "DashParam_AccelerationTime");
	param.dashParam.decelerationTime = gv->GetFloatValue(itemName_, "DashParam_DecelerationTime");

}

void Player::AddGlobalVariables(const MoveParam& param) {
	GlobalVariables* gv = GlobalVariables::GetInstance();

	param.inputMoveParam.AddGlobalVariables(itemName_, "InputMoveParam");
	gv->SetValue(itemName_, "InputMoveParam_AirMoveVelocity", param.airMoveVelocity);

	param.wireMoveParam.AddGlobalVariables(itemName_, "WireMoveParam");

	param.fallParam.AddGlobalVariables(itemName_, "FallParam");

	gv->SetValue(itemName_, "JumpFirstVelocity", param.jumpFirstVelocity);

	param.dashParam.speed.AddGlobalVariables(itemName_, "DashParam");
	
	gv->SetValue(itemName_, "DashParam_AccelerationTime", param.dashParam.accelerationTime);
	gv->SetValue(itemName_, "DashParam_DecelerationTime", param.dashParam.decelerationTime);

	gv->SaveFile(itemName_);
}
