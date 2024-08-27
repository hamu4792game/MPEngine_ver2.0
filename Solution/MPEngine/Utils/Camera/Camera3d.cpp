#include "Camera3d.h"

Camera3d* Camera3d::GetInstance() {
	static Camera3d instance;
	return &instance;
}

void Camera3d::Initialize(const float& farz) {
	camera_ = Camera(farz, true);
	camera_.transform.scale_ = Vector3::one;
	
}

Matrix4x4 Camera3d::GetViewProMat() {
	Matrix4x4 viewProMat = camera_.GetViewProMat();
	
	return viewProMat;
}
