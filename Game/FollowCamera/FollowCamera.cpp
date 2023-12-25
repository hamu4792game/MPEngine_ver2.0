#include "FollowCamera.h"
#include "externals/imgui/imgui.h"

void FollowCamera::Update() {
	DrawImGui();
	if (target_) {
		Vector3 offset = Vector3(0.0f, 2.0f, -20.0f);
		offset = TargetOffset(offset, transform_.rotation_);

		transform_.translation_ = offset + target_->translation_;
	}
	transform_.UpdateMatrix();
}

void FollowCamera::DrawImGui() {
	ImGui::Begin("camera");
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::End();
}
