#include "FollowCamera.h"
#include "externals/imgui/imgui.h"
#include "Input/Input.h"
#include "Utils/Easing/Easing.h"

void FollowCamera::Initialize(const WorldTransform& transform) {
	transform_.rotation_.x = AngleToRadian(5.0f);
	transform_ = transform;
	offset_ = Vector3(0.0f, 2.0f, -20.0f);
	preTranslate_ = target_->worldMatrix_ * TargetOffset(offset_, transform_.rotation_);
}

void FollowCamera::Update() {
	if (target_) {
		Vector3 lOffset = offset_;
		lOffset = TargetOffset(lOffset, transform_.rotation_);
		float T = Easing::EaseInSine(0.5f);

		Vector3 end = target_->worldMatrix_ * lOffset;

		preTranslate_ = Lerp(preTranslate_, end, T);

		transform_.translation_ = lOffset + preTranslate_;
	}
	transform_.UpdateMatrix();
}

void FollowCamera::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("camera");
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("offset", &offset_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG
}

void FollowCamera::CameraMove() {
	DrawImGui();
	auto input = Input::GetInstance();
	Vector2 move;
	if (input->GetKey()->PressKey(DIK_LEFT)) {
		move.y -= AngleToRadian(1.0f);
	}
	if (input->GetKey()->PressKey(DIK_RIGHT)) {
		move.y += AngleToRadian(1.0f);
	}
	if (input->GetKey()->PressKey(DIK_UP)) {
		move.x += AngleToRadian(1.0f);
	}
	if (input->GetKey()->PressKey(DIK_DOWN)) {
		move.x -= AngleToRadian(1.0f);
	}

	if (input->GetPad()->GetPadConnect()) {
		Vector2 pMove(0.0f, 0.0f);
		pMove = input->GetPad()->GetPadRStick();
		//	移動があれば代入する
		if (pMove.x < -0.5f) {
			move.y -= AngleToRadian(1.0f);
		}
		else if (pMove.x > 0.5f) {
			move.y += AngleToRadian(1.0f);
		}
		if (pMove.y < -0.5f) {
			move.x -= AngleToRadian(1.0f);
		}
		else if (pMove.y > 0.5f) {
			move.x += AngleToRadian(1.0f);
		}
	}
	transform_.rotation_.x += move.x;
	transform_.rotation_.y += move.y;

	transform_.rotation_.x = std::clamp(transform_.rotation_.x, AngleToRadian(-8.0f), AngleToRadian(75.0f));
}
