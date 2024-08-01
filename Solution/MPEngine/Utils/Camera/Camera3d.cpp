#include "Camera3d.h"

Camera3d* Camera3d::GetInstance() {
	static Camera3d instance;
	return &instance;
}

void Camera3d::Initialize(const float& farz) {
	camera_ = Camera(farz, true);
	camera_.transform.scale_ = Vector3::one;
	collision_ = std::make_unique<Collider>();
	collision_->Initialize(&camera_.transform, Collider::Type::Line);
}

Matrix4x4 Camera3d::GetViewProMat() {
	Matrix4x4 viewProMat = camera_.GetViewProMat();
	collision_->Update();
	return viewProMat;
}

bool Camera3d::OnCollision(const Collider& coll) {
	Vector3 pushBackVec;
	bool iscoll = collision_->OnCollision(coll, pushBackVec);
	if (iscoll) {
		// Groundと当たっていた場合
		if (coll.GetName() == "Ground") {
			if (Length(pushBackVec) <= Length(Vector3(0.0f, 2.0f, -20.0f))) {
				camera_.transform.translation_ -= pushBackVec;
				camera_.transform.UpdateMatrix();
				collision_->Update();
			}
		}
	}
	return false;
}
