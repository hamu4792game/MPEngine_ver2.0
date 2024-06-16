#include "PlayerMove.h"
#include "Utils/Camera/Camera3d.h"

PlayerMove::PlayerMove() {

}

void PlayerMove::Initialize(Vector3* modelRotate) {
	modelRotate_ptr = modelRotate;
}

bool PlayerMove::Update(const Vector3& direction) {
	isMoved_ = false;
	moveVector_ = Vector3::zero;

	// 通常状態かつ移動入力があれば
	if (true) {
		InputMove(direction);
	}

	if (isMoved_ == false) {
		return false;
	}

	return isMoved_;
}

void PlayerMove::InputMove(const Vector3& direction) {
	if (direction != Vector3::zero) {
		// 移動ベクトルに速度を足す 既に入力の状態で正規化されているのでそのまま
		const float speed = 0.5f;
		Vector3 move = direction * speed;

		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, Camera3d::GetInstance()->GetTransform().rotation_);
		move.y = 0.0f;
		moveVector_ += move;
		modelRotate_ptr->y = FindAngle(move, Vector3(0.0f, 0.0f, 1.0f));

		// 移動しているので
		isMoved_ = true;
	}
}
