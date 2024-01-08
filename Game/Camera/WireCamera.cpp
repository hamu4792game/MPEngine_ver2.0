#include "WireCamera.h"
#include "externals/imgui/imgui.h"
#include "Input/Input.h"
#include <algorithm>
#include "Utils/Easing/Easing.h"

void WireCamera::Initialize(const WorldTransform& transform) {
	//transform_.rotation_.x = AngleToRadian(5.0f);
	transform_ = transform;
	offset_ = Vector3(0.0f, 2.0f, -20.0f);
	preTranslate_ = target_->worldMatrix_ * TargetOffset(offset_, transform_.rotation_);
	easeNum_ = 0.0f;
	delay_ = 0.95f;
}

void WireCamera::Update() {
	if (target_) {
		Vector3 lOffset = offset_;
		lOffset = TargetOffset(lOffset, transform_.rotation_);
		
		Vector3 end = target_->worldMatrix_ * lOffset;

		preTranslate_ = Lerp(preTranslate_, end, 1.0f - delay_);

		transform_.translation_ = lOffset + preTranslate_;
	}
	transform_.UpdateMatrix();
}

void WireCamera::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("camera");
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG
}

void WireCamera::CameraMove() {
	DrawImGui();
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

	transform_.rotation_.x = std::clamp(transform_.rotation_.x, AngleToRadian(-8.0f), AngleToRadian(75.0f));

	easeNum_ += 1.0f / 20.0f;
	easeNum_ = std::clamp(easeNum_, 0.0f, 1.0f);
	float T = Easing::EaseOutCubic(easeNum_);
	T = std::clamp(T, 0.0f, 1.0f);
	const Vector3 kStartOffset(0.0f, 2.0f, -20.0f);
	const Vector3 kEndOffset(1.0f, 1.0f, -10.0f);
	
	if (changeFlag_) {
		offset_ = Lerp(kStartOffset, kEndOffset, T);
	}
	else {
		offset_ = Lerp(kEndOffset, kStartOffset, T);
		if (offset_ == kStartOffset) {
			delay_ -= 0.05f;
			delay_ = std::clamp(delay_, 0.0f, 0.95f);
		}
	}

}

void WireCamera::SetChangeFlag(const bool& flag) {
	changeFlag_ = flag;
	easeNum_ = 0.0f;
}
