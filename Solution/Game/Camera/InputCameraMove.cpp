#include "InputCameraMove.h"
#include "Input/Input.h"
#include <algorithm>

void inputCameraMove::Update() {
	auto input = Input::GetInstance();
	Vector2 move;
	float speed = 2.0f;
	// キーボード
	if (input->GetKey()->PressKey(DIK_LEFT)) {
		move.y -= AngleToRadian(speed);
	}
	if (input->GetKey()->PressKey(DIK_RIGHT)) {
		move.y += AngleToRadian(speed);
	}
	if (input->GetKey()->PressKey(DIK_UP)) {
		move.x += AngleToRadian(speed);
	}
	if (input->GetKey()->PressKey(DIK_DOWN)) {
		move.x -= AngleToRadian(speed);
	}

	// マウス
	Vector2 mouseMove = input->GetMouse()->GetMouseMove();
	static float cameraspeed = 0.2f;

	if (mouseMove != Vector2::zero) {
		move.y += AngleToRadian(mouseMove.x * cameraspeed);
		move.x += AngleToRadian(mouseMove.y * cameraspeed);
	}

	// pad
	if (input->GetPad()->GetPadConnect()) {
		Vector2 pMove(0.0f, 0.0f);
		pMove = input->GetPad()->GetPadRStick();
		const float kStickDeadZone = 0.5f;
		//	移動があれば代入する
		if (pMove.x < -kStickDeadZone) {
			move.y -= AngleToRadian(speed);
		}
		else if (pMove.x > kStickDeadZone) {
			move.y += AngleToRadian(speed);
		}
		if (pMove.y < -kStickDeadZone) {
			move.x += AngleToRadian(speed);
		}
		else if (pMove.y > kStickDeadZone) {
			move.x -= AngleToRadian(speed);
		}
	}

	// 上下左右の任意軸開店
	Quaternion yaw = Quaternion::MakeRotateAxisAngleQuaternion(Vector3::up, move.y);
	Quaternion pitch = Quaternion::MakeRotateAxisAngleQuaternion(Vector3::right, move.x);

	// 移動量
	transform_.rotationQuat_ = yaw * pitch;

}
