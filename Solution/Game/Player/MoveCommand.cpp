#include "MoveCommand.h"
#include "Utils/Camera/Camera3d.h"
#include "ImGuiManager/ImGuiManager.h"

MoveCommand::MoveCommand(const std::string& itemName) : itemName_(itemName) {
	
}

void MoveCommand::Initialize(MoveParam param, const float* masterSpeed) {
	param_ = param;
	wireTargetMove_ = std::make_unique<WireTargetMove>(&param_.wireMoveParam);
	masterSpeed_ptr = masterSpeed;
}

bool MoveCommand::UpInputNormalMove(Vector3 inputMove, WorldTransform& moveVolume) {
	// 移動入力している場合
	if (inputMove != Vector3::zero) {

		param_.inputMoveParam.AddUpdate();

		// 移動ベクトルに速度を足す 既に入力の状態で正規化されているのでそのまま
		float speed = param_.inputMoveParam.acceleration;
		Vector3 move = inputMove * speed;

		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, Camera3d::GetInstance()->GetTransform().rotation_);
		// y軸には移動しないため0を代入
		move.y = 0.0f;

		moveVolume.translation_ += move;
		moveVolume.rotationQuat_ = Quaternion::MakeFromTwoVector(Vector3::front,move.Normalize());
		//moveVolume.rotation_.y = FindAngle(move, Vector3::front);

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

bool MoveCommand::ExJump(FallParam& param) {
	// ジャンプが可能で、なおかつ落ちていない時
	if (param.isJumpable && !param.isFalled) {
		// 初速度を与える
		param.isJumpable = false;
		param.isFalled = true;
		param.fall.Initialize(1.0f, 0.05f, 0.0f, -3.0f);
		param.fall.acceleration = 1.0f;
	}
	return false;
}

Vector3 MoveCommand::UpFalling(FallParam& param) {
	Vector3 result;
	// 落下更新処理
	// 重力を足していく
	param.fall.acceleration -= param.fall.accelerationRate * *masterSpeed_ptr;
	// 速度制限
	//fallParam_.fall.acceleration = std::max(fallParam_.fall.acceleration, fallParam_.fall.kMaxAcceleration);
	result.y += param.fall.acceleration * *masterSpeed_ptr;
	if (param.isFalled) {
		if (param.fall.acceleration < 0.0f) {
			// 落下中
			param.isFalled = true;
		}
		else {
			// 上昇中
			param.isJumped = true;
		}
	}
	else {
		param.isFalled = false;
		param.isJumped = false;
	}
	return result;
}

void MoveCommand::ImGuiProc() {
#ifdef _DEBUG
	if (ImGui::TreeNode("Input")) {
		ImGui::DragFloat("accelerationRate", &param_.inputMoveParam.accelerationRate, 0.01f);
		ImGui::DragFloat("kMinAcceleration", &param_.inputMoveParam.kMinAcceleration, 0.01f);
		ImGui::DragFloat("kMaxAcceleration", &param_.inputMoveParam.kMaxAcceleration, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Wire")) {
		ImGui::DragFloat("accelerationRate", &param_.wireMoveParam.accelerationRate, 0.01f);
		ImGui::DragFloat("kMinAcceleration", &param_.wireMoveParam.kMinAcceleration, 0.01f);
		ImGui::DragFloat("kMaxAcceleration", &param_.wireMoveParam.kMaxAcceleration, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Fall")) {
		ImGui::DragFloat("accelerationRate", &param_.fallParam.accelerationRate, 0.01f);
		ImGui::DragFloat("kMinAcceleration", &param_.fallParam.kMinAcceleration, 0.01f);
		ImGui::DragFloat("kMaxAcceleration", &param_.fallParam.kMaxAcceleration, 0.01f);
		ImGui::TreePop();
	}

#endif // _DEBUG
}
