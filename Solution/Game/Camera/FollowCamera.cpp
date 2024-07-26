#include "FollowCamera.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Input/Input.h"
#include "Utils/Easing/Easing.h"

FollowCamera::FollowCamera() : kMaxOffset_(Vector3(0.0f, 2.0f, -30.0f)) {
	transform_.rotation_.x = AngleToRadian(5.0f);
	offset_ = Vector3(0.0f, 2.0f, -20.0f);
	preOffset_ = offset_;
	preRotate_ = transform_.rotation_;
	postRotate_ = transform_.rotation_;
}

void FollowCamera::Initialize(const WorldTransform& transform) {
	transform_ = transform;
	preTranslate_ = target_->worldMatrix_ * TargetOffset(offset_, transform_.rotation_);
	lerpSpeed_ = 0.2f;
}

void FollowCamera::SetParam(const Vector3& offset, const Vector3& rotate, float lerpSpeed) {
	offset_ = offset;
	postRotate_ = rotate;
	lerpSpeed_ = lerpSpeed;
}

void FollowCamera::Update(const float& speed) {
	if (target_) {
		Vector3 lOffset = offset_;
		
		lOffset.z = offset_.z - speed;

		preOffset_ = Lerp(preOffset_, lOffset, lerpSpeed_);
		preRotate_ = Lerp(preRotate_, postRotate_, lerpSpeed_);

		lOffset = TargetOffset(preOffset_, preRotate_);
		float T = Easing::EaseInSine(0.5f);

		Vector3 end = MakeTranslateMatrix(target_->GetPosition()) * lOffset;

		preTranslate_ = Lerp(preTranslate_, end, T);
		postTranslate_ = lOffset + end;

		transform_.translation_ = lOffset + preTranslate_;
		transform_.rotation_ = preRotate_;
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
	ImGui::DragFloat3("postRotate", &postRotate_.x, 0.1f);
	ImGui::DragFloat("lerpSpeed", &lerpSpeed_, 0.1f);
	ImGui::End();
#endif // _DEBUG
}

void FollowCamera::CameraMove() {
	DrawImGui();
	auto input = Input::GetInstance();
	Vector2 move;
	float speed = 2.0f;
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

	if (input->GetPad()->GetPadConnect()) {
		Vector2 pMove(0.0f, 0.0f);
		pMove = input->GetPad()->GetPadRStick();
		//	移動があれば代入する
		if (pMove.x < -0.5f) {
			move.y -= AngleToRadian(speed);
		}
		else if (pMove.x > 0.5f) {
			move.y += AngleToRadian(speed);
		}
		if (pMove.y < -0.5f) {
			move.x -= AngleToRadian(speed);
		}
		else if (pMove.y > 0.5f) {
			move.x += AngleToRadian(speed);
		}
	}
	//transform_.rotation_.x += move.x;
	//transform_.rotation_.y += move.y;
	postRotate_.x += move.x;
	postRotate_.y += move.y;

	static Vector2 limit = Vector2(-50.0f, 75.0f);
#ifdef _DEBUG
	ImGui::DragFloat2("Limit", &limit.x, 0.1f);
#endif // _DEBUG
	postRotate_.x = std::clamp(postRotate_.x, AngleToRadian(limit.x), AngleToRadian(limit.y));
}
