#include "MoveCommand.h"
#include "Utils/Camera/Camera3d.h"
#include "ImGuiManager/ImGuiManager.h"
#include <algorithm>
#include "Utils/Easing/Ease.h"
#undef min
#undef max

void MoveCommand::Initialize(MoveParam param, const float* masterSpeed) {
	param_ = param;
	masterSpeed_ptr = masterSpeed;
	wireTargetMove_ = std::make_unique<WireTargetMove>(&param_.wireMoveParam);
	webSwing_ = std::make_unique<WebSwing>();
}

bool MoveCommand::UpInputMove(Vector3 inputMove, WorldTransform& moveVolume, const bool& isLanded, const WorldTransform& cameraTransform) {
	// 移動入力している場合
	if (inputMove != Vector3::zero) {
		// 地に足ついてる時の処理
		if (isLanded) {
			param_.inputMoveParam.AddUpdate();
		}
		// 空中の処理
		else {
			param_.inputMoveParam.acceleration = param_.airMoveVelocity;
		}

		// 移動ベクトルに速度を足す 既に入力の状態で正規化されているのでそのまま
		float speed = param_.inputMoveParam.acceleration * *masterSpeed_ptr;
		Vector3 move = inputMove * speed;

		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, cameraTransform.rotation_);
		// y軸には移動しないため0を代入
		move.y = 0.0f;

		moveVolume.translation_ += move.Normalize() * speed;
		moveVolume.rotationQuat_ = Quaternion::MakeFromTwoVector(Vector3::front,move.Normalize());

		// 移動しているので
		return true;
	}
	else {
		// 動いていない
		param_.inputMoveParam.AccelInit();
		return false;
	}
}

void MoveCommand::ExWireTargetMove(const Vector3& target, const Vector3& player) {
	wireTargetMove_->Execute(target, player);
}

bool MoveCommand::UpWireTargetMove(const Vector3& player, Vector3& result) {
	return wireTargetMove_->Update(player, result);
}

void MoveCommand::ExJump(FallParam& param, const Vector3& direction) {
	// ジャンプが可能で、なおかつ落ちていない時
	if (param.isJumpable && !param.isFalled) {
		// 初速度を与える
		param.isJumpable = false;
		param.isFalled = true;
		param.fall.AccelInit(param_.jumpFirstVelocity);
		webswingDirection_ = Vector3::zero;
		jumpDirection_ = direction;
	}
}

Vector3 MoveCommand::UpFalling(FallParam& param) {
	Vector3 result = jumpDirection_;
	// 落下更新処理
	// 重力を足していく
	param.fall.acceleration -= param_.fallParam.accelerationRate * *masterSpeed_ptr;
	// 速度制限
	param.fall.acceleration = std::clamp(param.fall.acceleration, param_.fallParam.kMinAcceleration, param_.fallParam.kMaxAcceleration);
	result.y = param.fall.acceleration * *masterSpeed_ptr;
	if (param.isFalled) {
		if (param.fall.acceleration < 0.0f) {
			// 落下中
			param.isFalled = true;

			// 落下している間は真下にしか落ちない
			jumpDirection_ = Vector3::zero;
		}
		else {
			// 上昇中
			param.isJumped = true;

			// いったんなんも考えず代入。後で速度をかける
			result.x = jumpDirection_.x * *masterSpeed_ptr;
			result.z = jumpDirection_.z * *masterSpeed_ptr;

		}
	}
	else {
		param.isFalled = false;
		param.isJumped = false;
		// 落下している間は真下にしか落ちない
		jumpDirection_ = Vector3::zero;
	}
	return result;
}

void MoveCommand::ExWebSwing(const WorldTransform& player, const float& firstVel) {
	// playerの正面ベクトルを回転させた値 このマジックナンバーは後で消す
	Vector3 pl = player.rotationQuat_ * Vector3(0.0f, 15.0f, 5.0f);
	Vector3 pvec = player.rotationQuat_ * Vector3::front;

	webSwing_->Initialize(player.GetPosition() + pl, player.GetPosition(), pvec.Normalize() * firstVel);
}

Vector3 MoveCommand::UpWebSwing(const Vector3& playerPos, bool& isWebSwing) {
	isWebSwing = false;
	Vector3 moveVec = webSwing_->Update(playerPos, isWebSwing);
	// ウェブスイングが終わっていたら
	if (!isWebSwing) {
		// その向きに合わせて動き続ける
		webswingDirection_ = moveVec;
	}
	return moveVec;
}

Vector3 MoveCommand::UpPostWebSwing() {
	return Vector3(webswingDirection_.x, 0.0f, webswingDirection_.z);
}

Vector3 MoveCommand::UpWallMove(const Vector3& hitNormal, Quaternion& playerRotate, const Vector3& moveVec) {
	// そもそも移動されていなかったら早期リターン
	if (moveVec == Vector3::zero) { return Vector3::zero; }

	// 移動ベクトルをもとに正面方向のQuaternionを計算
	Quaternion forward = Quaternion::MakeFromTwoVector(Vector3::front, moveVec.Normalize());

	Vector3 lNormalVec = hitNormal.Normalize();
	// 角度の計算
	float angle = FindAngle(Vector3::up, lNormalVec);
	// 回転軸の計算
	Vector3 rotAxis = Cross(Vector3::up, lNormalVec).Normalize();

	// 壁に垂直なQuaternionを取得
	Quaternion qur = Quaternion::MakeRotateAxisAngleQuaternion(rotAxis, angle);
	// 移動ベクトルを回転
	Vector3 moveVector = qur * moveVec;
	// 疑似的な重力を使い壁に貼り付ける
	const float kSuctionPower = -0.2f; // 面に対しての吸引力
	moveVector += lNormalVec * kSuctionPower;

	// 下に移動しないよう制限
	if (moveVector.y <= 0.0f) {
		moveVector.y = 0.0f;
	}

	// 合成
	playerRotate = qur * forward;

	return moveVector;
}

Vector3 MoveCommand::UpSkyDash() {
	return Vector3();
}

Vector3 MoveCommand::ExDashStart(const Vector3& direction) {
	float time = 0.0f;
	float maxTime = 0.0f;
	// 初速度を与える
	if (true) {
		param_.dashParam.speed.AccelInit();
		
	}

	float maxAcc = 0.0f;
	// 加速時間なら
	if (time <= param_.dashParam.accelerationTime) {
		maxAcc = param_.dashParam.speed.kMaxAcceleration;
	}
	// 減速時間なら
	else if (time <= param_.dashParam.accelerationTime + param_.dashParam.decelerationTime) {
		maxAcc = param_.dashParam.speed.kMinAcceleration;
	}
	// 安定したら
	else {
		time = maxTime;
	}

	// 初速度から最大加速度まで、0から加速
	param_.dashParam.speed.acceleration = Ease::UseEase(param_.dashParam.speed.accelerationRate, maxAcc, time, maxTime, Ease::EaseType::EaseInCirc);

	return direction * param_.dashParam.speed.acceleration;
}

void MoveCommand::ImGuiProc() {
#ifdef _DEBUG
	if (ImGui::TreeNode("Input")) {
		if (ImGui::TreeNode("地面の基本速度")) {
			param_.inputMoveParam.ImGuiProc();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("空中スライド")) {
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Dash")) {
			param_.dashParam.speed.ImGuiProc();
			ImGui::DragFloat("加速時間", &param_.dashParam.accelerationTime, 0.1f);
			ImGui::DragFloat("減速時間", &param_.dashParam.decelerationTime, 0.1f);
			ImGui::TreePop();
		}
		
		ImGui::DragFloat("airMoveVelocity", &param_.airMoveVelocity, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Wire")) {
		param_.wireMoveParam.ImGuiProc();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Fall")) {
		param_.fallParam.ImGuiProc();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Jump")) {
		ImGui::DragFloat("jumpFirstVelocity", &param_.jumpFirstVelocity, 0.01f);
		ImGui::TreePop();
	}

#endif // _DEBUG
}
