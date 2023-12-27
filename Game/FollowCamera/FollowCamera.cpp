#include "FollowCamera.h"
#include "externals/imgui/imgui.h"
#include "Input/Input.h"

void FollowCamera::Initialize() {
	transform_.rotation_.x = AngleToRadian(5.0f);
}

void FollowCamera::Update() {
	DrawImGui();
	if (target_) {
		Vector3 offset = Vector3(0.0f, 4.0f, -30.0f);
		offset = TargetOffset(offset, transform_.rotation_);

		transform_.translation_ = offset + target_->translation_;
	}
	transform_.UpdateMatrix();
}

void FollowCamera::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("camera");
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG


	auto input = Input::GetInstance()->GetKey();
	if (input->PressKey(DIK_LEFT)) {
		transform_.rotation_.y -= AngleToRadian(1.0f);
	}
	if (input->PressKey(DIK_RIGHT)) {
		transform_.rotation_.y += AngleToRadian(1.0f);
	}
	if (input->PressKey(DIK_UP)) {
		transform_.rotation_.x += AngleToRadian(1.0f);
	}
	if (input->PressKey(DIK_DOWN)) {
		transform_.rotation_.x -= AngleToRadian(1.0f);
	}


}
