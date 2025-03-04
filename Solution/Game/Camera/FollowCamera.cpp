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

	collision_ = std::make_unique<Collider>();
	collision_->Initialize(&transform_, Collider::Type::Line, "followCamera");
}

void FollowCamera::Initialize(const WorldTransform& transform) {
	transform_ = transform;
	transform_.isQuaternion_ = true;

	preTranslate_ = target_->worldMatrix_ * TargetOffset(offset_, transform_.rotation_);
	lerpSpeed_ = 0.2f;

	collision_->GetLine().at(0)->SetLine(target_->GetPosition(), transform_.GetPosition());
	collision_->GetLine().at(0)->SetColor(0xff0000ff);
}

void FollowCamera::SetParam(const Vector3& offset, const Vector3& rotate, float lerpSpeed) {
	offset_ = offset;
	postRotate_ = rotate;
	lerpSpeed_ = lerpSpeed;
}

void FollowCamera::Update(const float& speed) {
	// offsetとtargetのカメラの更新

	timer_.nowFrame += 1.0f;
	if (timer_.nowFrame>= timer_.maxFrame) {
		timer_.nowFrame = timer_.maxFrame;
		isFollowStop_ = false;
	}

	if (target_ && !isFollowStop_) {
		Vector3 lOffset = offset_;
		
		lOffset.z = offset_.z - speed;

		preOffset_ = Lerp(preOffset_, lOffset, lerpSpeed_);
		preRotate_ = Lerp(preRotate_, postRotate_, lerpSpeed_);

		lOffset = TargetOffset(preOffset_, postRotate_);
		float T = Easing::EaseInSine(0.5f);

		Vector3 end = MakeTranslateMatrix(target_->GetPosition()) * lOffset;

		preTranslate_ = Lerp(preTranslate_, end, T);
		postTranslate_ = lOffset + end;

		transform_.translation_ = lOffset + end;
		transform_.rotation_ = postRotate_;
		transform_.rotationQuat_ = Quaternion::EulerToQuaternion(postRotate_);

		//transform_.rotationQuat_ = Quaternion::FromRotationMatrix4x4(MakeRotateMatrix(transform_.rotation_));
	}
	transform_.UpdateMatrix();
	if (target_) {
		collision_->Update(*target_);
	}
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

void FollowCamera::LastUpdate() {
	// カメラの遅延を含めた座標補間をここでやる
	
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

	Vector2 mouseMove = input->GetMouse()->GetMouseMove();
	static float cameraspeed = 0.2f;
#ifdef _DEBUG
	ImGui::DragFloat("CameraSpeed", &cameraspeed, 0.1f);
#endif // _DEBUG
	if (mouseMove != Vector2::zero) {
		move.y += AngleToRadian(mouseMove.x * cameraspeed);
		move.x += AngleToRadian(mouseMove.y * cameraspeed);
	}

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

void FollowCamera::StopFollow(const float& frame) {
	isFollowStop_ = true;
	timer_.maxFrame = frame;
	timer_.nowFrame = 0.0f;
}

bool FollowCamera::OnCollision(const Collider& coll) {
	Vector3 pushBackVec;
	// 当たっているかのフラグ
	bool iscoll = collision_->OnCollision(coll, pushBackVec);
	if (iscoll) {
		// Groundと当たっていた場合
		if (coll.GetName() == "Ground") {
			float push = Length(pushBackVec);
			float off = Length(transform_.GetPosition() - target_->GetPosition());
			if (push <= off) {
#ifdef _DEBUG
				ImGui::DragFloat3("IsHIT:PushBackVec", &pushBackVec.x, 0.1f);
				ImGui::Text("IsHIT");
#endif // DEBUG
				transform_.translation_ += pushBackVec;
				transform_.UpdateMatrix();
				collision_->Update(*target_);
			}
		}
	}
	return iscoll;
}
