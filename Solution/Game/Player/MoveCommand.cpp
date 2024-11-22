#include "MoveCommand.h"
#include "Utils/Camera/Camera3d.h"
#include "ImGuiManager/ImGuiManager.h"

MoveCommand::MoveCommand(const std::string& itemName) : itemName_(itemName) {

}

void MoveCommand::Initialize(MoveParam param, const float* masterSpeed) {
	param_ = param;
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
#endif // _DEBUG
}
